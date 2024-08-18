#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <QImage>
#include <QPainter>

#include <nel/misc/types_nl.h>
#include <nel/misc/file.h>
#include <nel/misc/o_xml.h>
#include <nel/misc/common.h>
#include <nel/misc/cmd_args.h>
#include <nel/misc/bitmap.h>
#include <nel/3d/zone.h>
#include <nel/3d/bezier_patch.h>
#include <nel/3d/landscape.h>
#include <nel/ligo/zone_region.h>

#include <libgltf/gltf.h>

#include <gltf-convert/zone/utils.h>

using namespace NL3D;
using namespace NLMISC;
using namespace NLLIGO;
using namespace std;

const uint8 TILE_LAYER_COUNT = 3;
const uint16 TILE_INFO_SIZE = 256;
const uint16 PATCH_SIZE = 16;
const uint16 NORMAL_SIZE = PATCH_SIZE * 4;
const uint16 NORMAL_MAP_SIZE = TILE_INFO_SIZE * 4;

struct TileData
{
	uint16 tileId;
	CUV uv;
};

struct VertexData
{
	CVector position;
	CVector normal;
	CUV tileInfoUv;
	TileData tile[TILE_LAYER_COUNT];
};
struct OutputData
{
	vector<VertexData> vertices;
};

uint8 getPatchTileIndex(const CPatch &patch, const uint8 s, const uint8 t)
{
	return t * patch.getOrderS() + s;
}

// check CTessFace::initTileUvRGBA for correct calculation
CUV tileOrientation(CUV in, uint8 orientation)
{
	switch (orientation)
	{
	default:
	case 0:
		return { in.U, in.V };
	case 1:
		return { 1 - in.V, in.U };
	case 2:
		return { 1 - in.U, 1 - in.V };
	case 3:
		return { in.V, 1 - in.U };
	}
}

CUV tileUV(const CUV &in, uint8 orientation, bool is256, uint8 uvOff)
{
	CUV out(tileOrientation(in, orientation));
	if (is256)
	{
		out *= 0.5;
		if (uvOff == 2 || uvOff == 3)
			out.U += 0.5;
		if (uvOff == 1 || uvOff == 2)
			out.V += 0.5;
	}
	// Do the HalfPixel scale bias.
	float hBiasXY, hBiasZ;
	if (is256)
	{
		hBiasXY = CLandscapeGlobals::TilePixelBias256;
		hBiasZ = CLandscapeGlobals::TilePixelScale256;
	}
	else
	{
		hBiasXY = CLandscapeGlobals::TilePixelBias128;
		hBiasZ = CLandscapeGlobals::TilePixelScale128;
	}

	// Scale the UV.
	out.U *= hBiasZ;
	out.V *= hBiasZ;
	out.U += hBiasXY;
	out.V += hBiasXY;

	return out;
}

void setPixel(QImage &image, int x, int y, uint16 grayscale)
{
	nlassert(image.format() == QImage::Format_Grayscale16);
	((uint16 *)image.scanLine(y))[x] = grayscale;
}

QImage createNormalMap(int width, int height)
{
	QImage image(width, height, QImage::Format_RGB32);
	image.fill(QColor::fromRgbF(0.0f, 0.0f, 1.0f));
	return image;
}

void drawNormalMap(const CPatch &patch, QImage &image)
{
	CBezierPatch bezierPatch;
	patch.unpack(bezierPatch);
	auto scale = image.width() / PATCH_SIZE;
	auto orderS = patch.getOrderS() * scale;
	auto orderT = patch.getOrderT() * scale;
	float OOS = 1.0f / (orderS - 1);
	float OOT = 1.0f / (orderT - 1);
	for (auto y = 0; y < orderT; y++)
	{
		for (auto x = 0; x < orderS; x++)
		{
			CVector normal(bezierPatch.evalNormal(x * OOS, y * OOT));
			image.setPixelColor(x, y, QColor::fromRgbF(normal.x, normal.y, normal.z));
		}
	}
}

void drawImage(QImage &target, int x, int y, QImage &part) {
	QPainter painter(&target);
	painter.drawImage(QPoint(x, y), part);
}

QImage createTileInfoMap(int width, int height)
{
	QImage image(width, height, QImage::Format_Grayscale16);
	image.fill(NL_TILE_ELM_LAYER_EMPTY);
	return image;
}

void drawTileInfoMap(const CPatch &patch, QImage &image, uint8 layer)
{
	const auto &tiles = patch.Tiles;
	for (auto y = 0; y < patch.getOrderT(); y++)
	{
		for (auto x = 0; x < patch.getOrderS(); x++)
		{
			auto tileIndex = getPatchTileIndex(patch, x, y);
			const auto &tile = tiles[tileIndex];
			setPixel(image, x, y, tile.Tile[layer]);
		}
	}
}
void buildFaces(CLandscape &landscape, sint zoneId, sint patch, OutputData &output, QImage *image, QImage &normalMap)
{
	CUV A(0, 0), B(0, 1), C(1, 1), D(1, 0);
	CZone *pZone = landscape.getZone(zoneId);

	// Then trace all patch.
	nlassert(patch >= 0);
	nlassert(patch < pZone->getNumPatchs());
	const CPatch *pa = const_cast<const CZone *>(pZone)->getPatch(patch);
	const auto &tiles = pa->Tiles;
	CBezierPatch bezierPatch;
	pa->unpack(bezierPatch);

	// Build the faces.
	//=================
	uint8 ordS = pa->getOrderS();
	uint8 ordT = pa->getOrderT();
	uint16 patchOffset(patch * PATCH_SIZE);
	uint8 x, y;
	float pixelOffset = 0.125f / TILE_INFO_SIZE;
	float OOS = 1.0f / ordS;
	float OOT = 1.0f / ordT;

	uint16 normal_offset_x((patch * NORMAL_SIZE) % normalMap.width()), normal_offset_y(((patch * NORMAL_SIZE) / normalMap.height()) * NORMAL_SIZE);
	QImage normalMapPatch = createNormalMap(NORMAL_SIZE, NORMAL_SIZE);
	drawNormalMap(*pa, normalMapPatch);
	drawImage(normalMap, normal_offset_x, normal_offset_y, normalMapPatch);

	uint16 offset_x(patchOffset % TILE_INFO_SIZE), offset_y((patchOffset / TILE_INFO_SIZE) * PATCH_SIZE);
	QImage tileInfoMapPatch = createTileInfoMap(TILE_INFO_SIZE, TILE_INFO_SIZE);
	drawTileInfoMap(*pa, tileInfoMapPatch, 0);
	drawImage(image[0], offset_x, offset_y, tileInfoMapPatch);
	drawTileInfoMap(*pa, tileInfoMapPatch, 1);
	drawImage(image[1], offset_x, offset_y, tileInfoMapPatch);
	drawTileInfoMap(*pa, tileInfoMapPatch, 2);
	drawImage(image[2], offset_x, offset_y, tileInfoMapPatch);

	for (y = 0; y < ordT; y++)
	{
		for (x = 0; x < ordS; x++)
		{
			auto tileIndex = getPatchTileIndex(*pa, x, y);
			const auto &tile = tiles[tileIndex];
			if (tile.Tile[0] == NL_TILE_ELM_LAYER_EMPTY)
			{
				nlwarning("tile base layer not defined patch %d x %d y %d tileIndex %d", patch, x, y, tileIndex);
			}
			uint16 imageX = offset_x + x;
			uint16 imageY = offset_y + y;
			CUV tileInfo(imageX, imageY);
			tileInfo.U /= TILE_INFO_SIZE;
			tileInfo.V /= TILE_INFO_SIZE;
			tileInfo.U += pixelOffset;
			tileInfo.V += pixelOffset;
			CUV a(tileInfo.U, tileInfo.V), b(tileInfo.U, tileInfo.V + pixelOffset), c(tileInfo.U + pixelOffset, tileInfo.V + pixelOffset), d(tileInfo.U + pixelOffset, tileInfo.V);
			CVector uvScaleBias;
			bool is256;
			uint8 uvOff;
			tile.getTile256Info(is256, uvOff);

			CVector va(pa->computeContinousVertex(x * OOS, y * OOT));
			CVector vb(pa->computeContinousVertex(x * OOS, (y + 1) * OOT));
			CVector vc(pa->computeContinousVertex((x + 1) * OOS, (y + 1) * OOT));
			CVector vd(pa->computeContinousVertex((x + 1) * OOS, y * OOT));
			CVector na(bezierPatch.evalNormal(x * OOS, y * OOT));
			CVector nb(bezierPatch.evalNormal(x * OOS, (y + 1) * OOT));
			CVector nc(bezierPatch.evalNormal((x + 1) * OOS, (y + 1) * OOT));
			CVector nd(bezierPatch.evalNormal((x + 1) * OOS, y * OOT));

			output.vertices.push_back({ .position = va,
			    .normal = na,
			    .tileInfoUv = a,
			    .tile = {
			        { .tileId = tile.Tile[0], .uv = tileUV(A, tile.getTileOrient(0), is256, uvOff) },
			        { .tileId = tile.Tile[1], .uv = tileUV(A, tile.getTileOrient(1), is256, uvOff) },
			        { .tileId = tile.Tile[2], .uv = tileUV(A, tile.getTileOrient(2), is256, uvOff) } } });
			output.vertices.push_back({ .position = vb,
			    .normal = nb,
			    .tileInfoUv = b,
			    .tile = {
			        { .tileId = tile.Tile[0], .uv = tileUV(B, tile.getTileOrient(0), is256, uvOff) },
			        { .tileId = tile.Tile[1], .uv = tileUV(B, tile.getTileOrient(1), is256, uvOff) },
			        { .tileId = tile.Tile[2], .uv = tileUV(B, tile.getTileOrient(2), is256, uvOff) } } });
			output.vertices.push_back({ .position = vc,
			    .normal = nc,
			    .tileInfoUv = c,
			    .tile = {
			        { .tileId = tile.Tile[0], .uv = tileUV(C, tile.getTileOrient(0), is256, uvOff) },
			        { .tileId = tile.Tile[1], .uv = tileUV(C, tile.getTileOrient(1), is256, uvOff) },
			        { .tileId = tile.Tile[2], .uv = tileUV(C, tile.getTileOrient(2), is256, uvOff) } } });

			output.vertices.push_back({ .position = va,
			    .normal = na,
			    .tileInfoUv = a,
			    .tile = {
			        { .tileId = tile.Tile[0], .uv = tileUV(A, tile.getTileOrient(0), is256, uvOff) },
			        { .tileId = tile.Tile[1], .uv = tileUV(A, tile.getTileOrient(1), is256, uvOff) },
			        { .tileId = tile.Tile[2], .uv = tileUV(A, tile.getTileOrient(2), is256, uvOff) } } });
			output.vertices.push_back({ .position = vc,
			    .normal = nc,
			    .tileInfoUv = c,
			    .tile = {
			        { .tileId = tile.Tile[0], .uv = tileUV(C, tile.getTileOrient(0), is256, uvOff) },
			        { .tileId = tile.Tile[1], .uv = tileUV(C, tile.getTileOrient(1), is256, uvOff) },
			        { .tileId = tile.Tile[2], .uv = tileUV(C, tile.getTileOrient(2), is256, uvOff) } } });
			output.vertices.push_back({ .position = vd,
			    .normal = nd,
			    .tileInfoUv = d,
			    .tile = {
			        { .tileId = tile.Tile[0], .uv = tileUV(D, tile.getTileOrient(0), is256, uvOff) },
			        { .tileId = tile.Tile[1], .uv = tileUV(D, tile.getTileOrient(1), is256, uvOff) },
			        { .tileId = tile.Tile[2], .uv = tileUV(D, tile.getTileOrient(2), is256, uvOff) } } });
		}
	}
}

std::string getLongArgFirstValue(const NLMISC::CCmdArgs &args, const std::string &argName)
{
	std::string firstValue;
	const auto values = args.getLongArg(argName);
	if (!values.empty())
	{
		firstValue = values.front();
	}
	return firstValue;
}

void addZone(CLandscape &landscape, const std::string &zoneSearchDirectory, const sint x, const sint y)
{
	std::string zoneFilename(zoneSearchDirectory);
	zoneFilename += zoneName(x, y);
	zoneFilename += ".zonel";

	CIFile zoneFile;
	if (zoneFile.open(zoneFilename))
	{
		nlinfo("Found Neighbor Zone: %s", zoneFilename.c_str());
		CZone zone;
		zone.serial(zoneFile);
		landscape.addZone(zone);
		zoneFile.close();
		return;
	}

	zoneFilename = zoneSearchDirectory;
	zoneFilename += zoneNameLowerCase(x, y);
	zoneFilename += ".zonel";
	if (zoneFile.open(zoneFilename))
	{
		nlinfo("Found Neighbor Zone: %s", zoneFilename.c_str());
		CZone zone;
		zone.serial(zoneFile);
		landscape.addZone(zone);
		zoneFile.close();
	}
}

void addNeighborZones(CLandscape &landscape, const uint16 &zoneId, const std::string &zoneSearchDirectory)
{
	const sint x(zoneId & 255);
	const sint y(zoneId >> 8);

	addZone(landscape, zoneSearchDirectory, x - 1, y - 1);
	addZone(landscape, zoneSearchDirectory, x + 0, y - 1);
	addZone(landscape, zoneSearchDirectory, x + 1, y - 1);
	addZone(landscape, zoneSearchDirectory, x - 1, y + 0);
	addZone(landscape, zoneSearchDirectory, x + 0, y + 0);
	addZone(landscape, zoneSearchDirectory, x + 1, y + 0);
	addZone(landscape, zoneSearchDirectory, x - 1, y + 1);
	addZone(landscape, zoneSearchDirectory, x + 0, y + 1);
	addZone(landscape, zoneSearchDirectory, x + 1, y + 1);
}

std::optional<std::string> openFile(COFile &file, const std::string &directory, const std::string &basename, const std::string &suffix)
{
	std::string fileName = basename + suffix;
	std::string filePath = directory + "/" + fileName;
	if (!file.open(filePath, false, false, false))
	{
		nlwarning("Can't open the file for writing: %s", filePath.c_str());
	}

	return fileName;
}

int main(int argc, char **argv)
{
	try
	{
		NLMISC::CApplicationContext myApplicationContext;
		NLMISC::CCmdArgs args;
		QImage tileInfo[TILE_LAYER_COUNT] = {
			createTileInfoMap(TILE_INFO_SIZE, TILE_INFO_SIZE),
			createTileInfoMap(TILE_INFO_SIZE, TILE_INFO_SIZE),
			createTileInfoMap(TILE_INFO_SIZE, TILE_INFO_SIZE)
		};

		args.addAdditionalArg("input", ".zonel Input zone file");
		args.addAdditionalArg("output", "Output gltf file");
		args.addArg("", "tile-bank", "[name.smallbank]", "TileBank to load");
		args.addArg("", "use-relative-position", "", "Use position relative to zone, not global world position");
		args.addArg("", "image-prefix", "path", "prefix to add for image uris");
		args.addArg("", "image-extension", "ext", "file extension to use for images");

		if (!args.parse(argc, argv))
		{
			args.displayHelp();
			return EXIT_FAILURE;
		}

		std::string inputFilePath = args.getAdditionalArg("input").front();
		std::string zoneSearchDirectory = CFile::getPath(inputFilePath);
		std::string bankFilePath = getLongArgFirstValue(args, "tile-bank");
		std::string outputFilePath = args.getAdditionalArg("output").front();
		std::string outputDirectory = CFile::getPath(outputFilePath);
		std::string basename = CFile::getFilenameWithoutExtension(outputFilePath);
		std::string positionFilename = basename + ".position.bin";
		std::string positionFilePath = outputDirectory + "/" + positionFilename;
		std::string normalFilename = basename + ".normal.bin";
		std::string normalFilePath = outputDirectory + "/" + normalFilename;
		std::string imageUriPrefix = getLongArgFirstValue(args, "image-prefix");
		std::string imageFileExtension = getLongArgFirstValue(args, "image-extension");
		bool useRelativePosition = args.haveLongArg("use-relative-position");

		CIFile zoneFile;
		if (!zoneFile.open(inputFilePath))
		{
			nlwarning("Can't open the file for reading: %s", inputFilePath.c_str());
			return EXIT_FAILURE;
		}
		CLandscape landscape;
		CAABBox bbox;
		CZone loadingZone;
		loadingZone.serial(zoneFile);
		zoneFile.close();
		const auto zoneId(loadingZone.getZoneId());
		landscape.setNoiseMode(false);
		// add neighbor zones to get the same border vertices
		addNeighborZones(landscape, zoneId, zoneSearchDirectory);
		auto zone = landscape.getZone(zoneId);
		if (zone == nullptr)
		{
			nlerror("Can't finde zone with id: %i", zoneId);
			return EXIT_FAILURE;
		}
		COFile outputPosition;
		try
		{
			if (!bankFilePath.empty())
			{
				CIFile bankFile(bankFilePath);
				auto &tileBank = landscape.TileBank;
				tileBank.serial(bankFile);
				nldebug("TileBank land count %i", tileBank.getLandCount());
				nldebug("TileBank tileSet count %i", tileBank.getTileSetCount());
				nldebug("TileBank tile count %i", tileBank.getTileCount());
			}
		}
		catch (const Exception &)
		{
			nlerror("Can't load bankfile: %s", bankFilePath.c_str());
			return EXIT_FAILURE;
		}
		if (!outputPosition.open(positionFilePath, false, false, false))
		{
			nlwarning("Can't open the file for writing: %s", positionFilePath.c_str());
			return EXIT_FAILURE;
		}
		COFile outputNormal;
		if (!outputNormal.open(normalFilePath, false, false, false))
		{
			nlwarning("Can't open the file for writing: %s", normalFilePath.c_str());
			return EXIT_FAILURE;
		}
		COFile texCoord0Output;
		auto textCoord0Filename = openFile(texCoord0Output, outputDirectory, basename, ".texcoord_0.bin");
		if (!textCoord0Filename)
		{
			return EXIT_FAILURE;
		}
		COFile texCoord1Output;
		auto texCoord1Filename = openFile(texCoord1Output, outputDirectory, basename, ".texcoord_1.bin");
		if (!texCoord1Filename)
		{
			return EXIT_FAILURE;
		}

		const sint zoneX(zoneId & 255);
		const sint zoneY(zoneId >> 8);
		CVector zoneOffset(160.0f * zoneX, -160.0f * zoneY, 0.0f);
		gltf::Mesh mesh = { .name = zoneName(zoneX, zoneY) };
		gltf::Node node = { .name = mesh.name, .mesh = 0, .translation = { zoneOffset.x, zoneOffset.y, zoneOffset.z } };
		if (useRelativePosition)
		{
			node.translation.clear();
		}
		gltf::Asset asset = {
			.nodes = { node },
			.scenes = { { .nodes = { 0 } } }
		};
		OutputData output;
		QImage normalMap = createNormalMap(NORMAL_MAP_SIZE, NORMAL_MAP_SIZE);
		for (sint patchIndex = 0; patchIndex < zone->getNumPatchs(); patchIndex++)
		{
			buildFaces(landscape, zoneId, patchIndex, output, tileInfo, normalMap);
		}
		tileInfo[0].save(QString::fromStdString(outputDirectory + "/" + basename + ".tile-id-0.png"));
		tileInfo[1].save(QString::fromStdString(outputDirectory + "/" + basename + ".tile-id-1.png"));
		tileInfo[2].save(QString::fromStdString(outputDirectory + "/" + basename + ".tile-id-2.png"));
		normalMap.save(QString::fromStdString(outputDirectory + "/" + basename + ".normal.png"));
		for (auto &vertex : output.vertices)
		{
			vertex.position -= zoneOffset;
			vertex.position.serial(outputPosition);

			vertex.normal.serial(outputNormal);

			vertex.tile[0].uv.serial(texCoord0Output);

			// use texture for tile ids
			 vertex.tileInfoUv.serial(texCoord1Output);
			// use UV for tile ids
//			CUV tileIds(vertex.tile[0].tileId, vertex.tile[1].tileId);
//			tileIds.serial(texCoord1Output);
		}
		gltf::Primitive primitive = { .attributes = {} };
		gltf::Accessor position = gltf::Accessor::position(0, 0, output.vertices.size());
		gltf::Accessor normal = gltf::Accessor::normal(1, 0, output.vertices.size());
		gltf::Accessor texcoord0 = { .bufferView = 2, .byteOffset = 0, .componentType = gltf::ComponentType::FLOAT, .count = output.vertices.size(), .type = gltf::AccessorType::VEC2 };
		gltf::Accessor tileId = { .bufferView = 3, .byteOffset = 0, .componentType = gltf::ComponentType::FLOAT, .count = output.vertices.size(), .type = gltf::AccessorType::VEC2 };
		primitive.attributes.position = asset.accessors.size();
		asset.accessors.push_back(position);

		primitive.attributes.normal = asset.accessors.size();
		asset.accessors.push_back(normal);

		primitive.attributes.texcoord0 = asset.accessors.size();
		asset.accessors.push_back(texcoord0);

		primitive.attributes.texcoord1 = asset.accessors.size();
		asset.accessors.push_back(tileId);

		mesh.primitives.push_back(primitive);
		asset.meshes.push_back(mesh);

		FILE *fp = nlfopen(outputFilePath, "w");
		if (fp == NULL)
		{
			nlwarning("Can't open the file for writing: %s", outputFilePath.c_str());
			return EXIT_FAILURE;
		}
		gltf::JsonWriter gltfWriter = { .file = fp };
		asset.bufferViews.push_back({ .buffer = asset.buffers.size(), .byteLength = outputPosition.getPos() });
		asset.buffers.push_back({ .uri = positionFilename, .byteLength = outputPosition.getPos() });
		asset.bufferViews.push_back({ .buffer = asset.buffers.size(), .byteLength = outputNormal.getPos() });
		asset.buffers.push_back({ .uri = normalFilename, .byteLength = outputNormal.getPos() });
		asset.bufferViews.push_back({ .buffer = asset.buffers.size(), .byteLength = texCoord0Output.getPos() });
		asset.buffers.push_back({ .uri = textCoord0Filename.value(), .byteLength = texCoord0Output.getPos() });
		asset.bufferViews.push_back({ .buffer = asset.buffers.size(), .byteLength = texCoord1Output.getPos() });
		asset.buffers.push_back({ .uri = texCoord1Filename.value(), .byteLength = texCoord1Output.getPos() });
		gltfWriter.write(asset);
		fclose(fp);
		outputPosition.close();
		outputNormal.close();
		texCoord0Output.close();
		texCoord1Output.close();

		return EXIT_SUCCESS;
	}
	catch (const Exception &e)
	{
		nlwarning("Error in writing zone file: %s", e.what());
		return EXIT_FAILURE;
	}
}
