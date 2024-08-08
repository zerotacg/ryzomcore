#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

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

struct VertexData
{
	CVector position;
	CVector normal;
	CUV uv;
	uint16 tileId;
	uint16 tileId1;
};
struct OutputData
{
	vector<VertexData> vertices;
};

uint8 getPatchTileIndex(const CPatch &patch, const uint8 s, const uint8 t)
{
	return t * patch.getOrderS() + s;
}

CUV tileOrienation(CUV in, uint8 orientation)
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

CUV tileUV(CUV in, uint8 orientation, bool is256, uint8 uvOff)
{
	CUV out(tileOrienation(in, orientation));
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

void buildFaces(CLandscape &landscape, sint zoneId, sint patch, OutputData &output)
{
	output.vertices.clear();
	CZone *pZone = landscape.getZone(zoneId);

	// Then trace all patch.
	sint N = pZone->getNumPatchs();
	nlassert(patch >= 0);
	nlassert(patch < N);
	const CPatch *pa = const_cast<const CZone *>(pZone)->getPatch(patch);
	const auto &tiles = pa->Tiles;
	CBezierPatch bezierPatch;
	pa->unpack(bezierPatch);

	// Build the faces.
	//=================
	sint ordS = pa->getOrderS();
	sint ordT = pa->getOrderT();
	sint x, y;
	float OOS = 1.0f / ordS;
	float OOT = 1.0f / ordT;
	for (y = 0; y < ordT; y++)
	{
		for (x = 0; x < ordS; x++)
		{
			auto tileIndex = getPatchTileIndex(*pa, x, y);
			const auto &tile = tiles[tileIndex];
			auto tileId = tile.Tile[0];
			auto tileId1 = tile.Tile[1];
			if (tileId == NL_TILE_ELM_LAYER_EMPTY)
			{
				nlwarning("tile base layer not defined patch %d x %d y %d tileIndex %d", patch, x, y, tileIndex);
			}
			// check CTessFace::initTileUvRGBA for correct calculation
			uint8 orientation = tile.getTileOrient(0);
			CVector uvScaleBias;
			bool is256;
			uint8 uvOff;
			tile.getTile256Info(is256, uvOff);
			//			CUV a(x * OOS, y * OOT), b(x * OOS, (y + 1) * OOT), c((x + 1) * OOS, (y + 1) * OOT), d((x + 1) * OOS, y * OOT);
			//			CUV a(0, 0), b(0, 1), c(1, 1), d(1, 0);
			CUV a(0, 0), b(0, 1), c(1, 1), d(1, 0);
			a = tileUV(a, orientation, is256, uvOff);
			b = tileUV(b, orientation, is256, uvOff);
			c = tileUV(c, orientation, is256, uvOff);
			d = tileUV(d, orientation, is256, uvOff);

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
			    .uv = a,
			    .tileId = tileId, .tileId1 = tileId1 });
			output.vertices.push_back({ .position = vb,
			    .normal = nb,
			    .uv = b,
			    .tileId = tileId, .tileId1 = tileId1 });
			output.vertices.push_back({ .position = vc,
			    .normal = nc,
			    .uv = c,
			    .tileId = tileId, .tileId1 = tileId1 });

			output.vertices.push_back({ .position = va,
			    .normal = na,
			    .uv = a,
			    .tileId = tileId, .tileId1 = tileId1 });
			output.vertices.push_back({ .position = vc,
			    .normal = nc,
			    .uv = c,
			    .tileId = tileId, .tileId1 = tileId1 });
			output.vertices.push_back({ .position = vd,
			    .normal = nd,
			    .uv = d,
			    .tileId = tileId, .tileId1 = tileId1 });
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

int main(int argc, char **argv)
{
	try
	{
		NLMISC::CApplicationContext myApplicationContext;
		NLMISC::CCmdArgs args;

		args.addAdditionalArg("input", ".zonel Input zone file");
		args.addAdditionalArg("output", "Output gltf file");
		args.addArg("", "tile-bank", "[name.smallbank]", "TileBank to load");
		args.addArg("", "use-relative-position", "", "Use position relative to zone, not global world position");
		args.addArg("", "use-tile-id-channel", "", "Use texture channel for tile ids instead of materials");
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
		std::string fileName = CFile::getFilenameWithoutExtension(outputFilePath);
		std::string positionFileName = fileName + ".position.bin";
		std::string positionFilePath = outputDirectory + "/" + positionFileName;
		std::string normalFileName = fileName + ".normal.bin";
		std::string normalFilePath = outputDirectory + "/" + normalFileName;
		std::string textureCoordinateFileName = fileName + ".texcoord.bin";
		std::string textureCoordinateFilePath = outputDirectory + "/" + textureCoordinateFileName;
		std::string tileIdFileName = fileName + ".tile-id.bin";
		std::string tileIdFilePath = outputDirectory + "/" + tileIdFileName;
		std::string imageUriPrefix = getLongArgFirstValue(args, "image-prefix");
		std::string imageFileExtension = getLongArgFirstValue(args, "image-extension");
		bool useRelativePosition = args.haveLongArg("use-relative-position");
		bool useTileIdChannel = args.haveLongArg("use-tile-id-channel");

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
		std::vector<gltf::Image> images;
		std::vector<gltf::Texture> textures;
		std::vector<gltf::Material> materials;
		std::map<std::string, size_t> filenameToTextureIndex;
		std::map<uint16, size_t> tileIdToTexture;
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
				if (!useTileIdChannel)
				{
					for (auto tileId = 0; tileId < tileBank.getTileCount(); ++tileId)
					{
						auto tile = tileBank.getTile(tileId);
						std::string imageUri = tile->getFileName(CTile::diffuse);
						auto foundImage = filenameToTextureIndex.find(imageUri);
						if (tile->isFree())
						{
							continue;
						}
						if (foundImage == filenameToTextureIndex.end())
						{
							gltf::Texture texture = { .source = images.size() };
							filenameToTextureIndex[imageUri] = tileIdToTexture[tileId] = textures.size();
							if (!imageFileExtension.empty())
							{
								auto imageFileName = CFile::getFilenameWithoutExtension(imageUri);
								imageFileName += ".";
								imageFileName += imageFileExtension;
								imageUri = CFile::getPath(imageUri);
								imageUri += imageFileName;
							}
							std::replace(imageUri.begin(), imageUri.end(), '\\', '/');
							images.push_back({ .uri = imageUriPrefix + imageUri });
							textures.push_back(texture);
						}
						else
						{
							tileIdToTexture[tileId] = foundImage->second;
						}
						materials.push_back(gltf::Material { .name = materialName(tileId), .pbrMetallicRoughness = gltf::MetallicRoughness { tileIdToTexture[tileId] } });
					}
				}
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
		COFile outputTextureCoordinate;
		if (!outputTextureCoordinate.open(textureCoordinateFilePath, false, false, false))
		{
			nlwarning("Can't open the file for writing: %s", textureCoordinateFilePath.c_str());
			return EXIT_FAILURE;
		}
		COFile outputTileId;
		if (!outputTileId.open(tileIdFilePath, false, false, false))
		{
			nlwarning("Can't open the file for writing: %s", tileIdFilePath.c_str());
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
			.materials = materials,
			.textures = textures,
			.images = images,
			.nodes = { node },
			.scenes = { { .nodes = { 0 } } }
		};
		for (sint patchIndex = 0; patchIndex < zone->getNumPatchs(); patchIndex++)
		{
			const CPatch *patch = static_cast<const CZone *>(zone)->getPatch(patchIndex);
			OutputData output;

			buildFaces(landscape, zoneId, patchIndex, output);

			size_t verticesPerTile = 6;

			gltf::Primitive primitive = { .attributes = {} };
			gltf::Accessor position = { .bufferView = 0, .componentType = gltf::ComponentType::FLOAT, .count = verticesPerTile, .type = gltf::AccessorType::VEC3 };
			gltf::Accessor normal = { .bufferView = 1, .componentType = gltf::ComponentType::FLOAT, .count = verticesPerTile, .type = gltf::AccessorType::VEC3 };
			gltf::Accessor texcoord0 = { .bufferView = 2, .componentType = gltf::ComponentType::FLOAT, .count = verticesPerTile, .type = gltf::AccessorType::VEC2 };
			gltf::Accessor texcoord1 = { .bufferView = 3, .componentType = gltf::ComponentType::FLOAT, .count = verticesPerTile, .type = gltf::AccessorType::VEC2 };
			auto vertex = output.vertices.begin();
			for (auto &texture : patch->Tiles)
			{
				auto tileId = texture.Tile[0];
				if (!useTileIdChannel && tileId != NL_TILE_ELM_LAYER_EMPTY)
				{
					const auto name = materialName(tileId);
					if (!bankFilePath.empty())
					{
						for (auto i = 0; i < asset.materials.size(); ++i)
						{
							auto &material(asset.materials[i]);
							if (material.name == name)
							{
								primitive.material = i;
							}
						}
					}
					else
					{
						primitive.material = asset.materials.size();
						for (auto i = 0; i < asset.materials.size(); ++i)
						{
							if (asset.materials[i].name == name)
							{
								primitive.material = i;
							}
						}
						if (primitive.material == asset.materials.size())
						{
							asset.materials.push_back({ .name = name });
						}
					}
				}
				primitive.attributes.position = asset.accessors.size();
				position.byteOffset = outputPosition.getPos();
				asset.accessors.push_back(position);

				primitive.attributes.normal = asset.accessors.size();
				normal.byteOffset = outputNormal.getPos();
				asset.accessors.push_back(normal);

				primitive.attributes.texcoord0 = asset.accessors.size();
				texcoord0.byteOffset = outputTextureCoordinate.getPos();
				asset.accessors.push_back(texcoord0);

				primitive.attributes.texcoord1 = asset.accessors.size();
				texcoord1.byteOffset = outputTileId.getPos();
				asset.accessors.push_back(texcoord1);

				for (auto i = 0; i < verticesPerTile && vertex != output.vertices.end(); ++i, ++vertex)
				{
					vertex->position -= zoneOffset;
					vertex->position.serial(outputPosition);

					vertex->normal.serial(outputNormal);

					vertex->uv.serial(outputTextureCoordinate);

					float u(vertex->tileId);
					float v(vertex->tileId1);
					outputTileId.serial(u);
					outputTileId.serial(v);
				}
				mesh.primitives.push_back(primitive);
			}
		}
		asset.meshes.push_back(mesh);

		FILE *fp = nlfopen(outputFilePath, "w");
		if (fp == NULL)
		{
			nlwarning("Can't open the file for writing: %s", outputFilePath.c_str());
			return EXIT_FAILURE;
		}
		gltf::JsonWriter gltfWriter = { .file = fp };
		asset.bufferViews.push_back({ .buffer = asset.buffers.size(), .byteLength = outputPosition.getPos() });
		asset.buffers.push_back({ .uri = positionFileName, .byteLength = outputPosition.getPos() });
		asset.bufferViews.push_back({ .buffer = asset.buffers.size(), .byteLength = outputNormal.getPos() });
		asset.buffers.push_back({ .uri = normalFileName, .byteLength = outputNormal.getPos() });
		asset.bufferViews.push_back({ .buffer = asset.buffers.size(), .byteLength = outputTextureCoordinate.getPos() });
		asset.buffers.push_back({ .uri = textureCoordinateFileName, .byteLength = outputTextureCoordinate.getPos() });
		if (useTileIdChannel)
		{
			asset.bufferViews.push_back({ .buffer = asset.buffers.size(), .byteLength = outputTileId.getPos() });
			asset.buffers.push_back({ .uri = tileIdFileName, .byteLength = outputTileId.getPos() });
		}
		gltfWriter.write(asset);
		fclose(fp);
		outputPosition.close();
		outputNormal.close();
		outputTextureCoordinate.close();
		outputTileId.close();

		return EXIT_SUCCESS;
	}
	catch (const Exception &e)
	{
		nlwarning("Error in writing zone file: %s", e.what());
		return EXIT_FAILURE;
	}
}
