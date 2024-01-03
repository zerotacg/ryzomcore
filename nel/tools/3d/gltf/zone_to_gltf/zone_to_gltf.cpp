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
#include <nel/3d/landscape.h>
#include <nel/ligo/zone_region.h>

#include <libgltf/gltf.h>

using namespace NL3D;
using namespace NLMISC;
using namespace NLLIGO;
using namespace std;

void buildFaces(CLandscape &landscape, sint zoneId, sint patch, std::vector<CVector> &vertices, std::vector<CUV> &textureCordinates)
{
	vertices.clear();
	CZone *pZone = landscape.getZone(zoneId);

	// Then trace all patch.
	sint N = pZone->getNumPatchs();
	nlassert(patch >= 0);
	nlassert(patch < N);
	const CPatch *pa = const_cast<const CZone *>(pZone)->getPatch(patch);

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
			CUV a(x * OOS, y * OOT), b(x * OOS, (y + 1) * OOT), c((x + 1) * OOS, (y + 1) * OOT), d((x + 1) * OOS, y * OOT);
			// CUV a(0, 0), b(0, 1), c(1, 1), d(1, 0);
			CVector va(pa->computeContinousVertex(x * OOS, y * OOT));
			CVector vb(pa->computeContinousVertex(x * OOS, (y + 1) * OOT));
			CVector vc(pa->computeContinousVertex((x + 1) * OOS, (y + 1) * OOT));
			CVector vd(pa->computeContinousVertex((x + 1) * OOS, y * OOT));

			vertices.push_back(va);
			textureCordinates.push_back(a);
			vertices.push_back(vb);
			textureCordinates.push_back(b);
			vertices.push_back(vc);
			textureCordinates.push_back(c);

			vertices.push_back(va);
			textureCordinates.push_back(a);
			vertices.push_back(vc);
			textureCordinates.push_back(c);
			vertices.push_back(vd);
			textureCordinates.push_back(d);
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

std::string zoneName(const sint x, const sint y)
{
	std::ostringstream name;

	name << y + 1 << "_" << static_cast<char>('A' + (x / 26)) << static_cast<char>('A' + (x % 26));

	return name.str();
}

std::string materialName(const uint16 tileId)
{
	std::ostringstream name;

	name << "M_tile_id_" << tileId;

	return name.str();
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

void validatePatchVertice(const CVector &vertex, const float scale, const CVector &zoneOffset)
{
	auto normalized = vertex - zoneOffset;
	const float delta = 0.5f;
	if (normalized.x < -delta || 160.0f + delta < normalized.x)
	{
		nlwarning("vertex is outside of zone grid %f %f %f", normalized.x, normalized.y, normalized.z);
	}
	// else if (normalized.x < scale || (160.0f - scale) < normalized.x)
	// {
	// 	nlwarning("vertex is close to zone grid %f %f %f", normalized.x, normalized.y, normalized.z);
	// }
	else if (normalized.y < -160.0f - delta || delta < normalized.y)
	{
		nlwarning("vertex is outside of zone grid %f %f %f", normalized.x, normalized.y, normalized.z);
	}
	// else if (normalized.y < (-160.0f + scale) || -scale < normalized.y)
	// {
	// 	nlwarning("vertex is close to zone grid %f %f %f", normalized.x, normalized.y, normalized.z);
	// }
}

void clampVertice(CVector &vertex)
{
	const float delta(0.125);
	vertex.x = std::clamp(vertex.x, 0.0f, 160.f);
	if (vertex.x < delta)
	{
		vertex.x = 0.0f;
	}
	else if (vertex.x > (160.0f - delta))
	{
		vertex.x = 160.0f;
	}

	vertex.y = std::clamp(vertex.y, -160.0f, 0.f);
	if (vertex.y > -delta)
	{
		vertex.y = 0.0f;
	}
	else if (vertex.y < (-160.0f + delta))
	{
		vertex.y = -160.0f;
	}
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
		std::string textureCordinateFileName = fileName + ".texcoord.bin";
		std::string textureCordinateFilePath = outputDirectory + "/" + textureCordinateFileName;
		std::string imageUriPrefix = getLongArgFirstValue(args, "image-prefix");
		std::string imageFileExtension = getLongArgFirstValue(args, "image-extension");
		bool useRelativePosion = args.haveLongArg("use-relative-position");

		CIFile zoneFile;
		if (!zoneFile.open(inputFilePath))
		{
			nlwarning("Can't open the file for reading: %s", inputFilePath.c_str());
			return EXIT_FAILURE;
		}
		CLandscape landscape;
		bool firstVertex = true;
		CAABBox bbox;
		CZone loadingZone;
		loadingZone.serial(zoneFile);
		zoneFile.close();
		const auto zoneId(loadingZone.getZoneId());
		landscape.setNoiseMode(false);
		// add neighbor zones to get the same border vertices
		addNeighborZones(landscape, zoneId, zoneSearchDirectory);
		auto zone = landscape.getZone(zoneId);
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
				for (auto tileId = 0; tileId < tileBank.getTileCount(); ++tileId)
				{
					auto tile = tileBank.getTile(tileId);
					std::string imageUri = tile->getFileName(CTile::diffuse);
					auto foundImage = filenameToTextureIndex.find(imageUri);
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
		COFile outputTextureCordinate;
		if (!outputTextureCordinate.open(textureCordinateFilePath, false, false, false))
		{
			nlwarning("Can't open the file for writing: %s", textureCordinateFilePath.c_str());
			return EXIT_FAILURE;
		}

		const sint zoneX(zoneId & 255);
		const sint zoneY(zoneId >> 8);
		CVector zoneOffset(160.0f * zoneX, -160.0f * zoneY, 0.0f);
		gltf::Mesh mesh;
		gltf::Node node = { .name = zoneName(zoneX, zoneY), .mesh = 0, .translation = { zoneOffset.x, zoneOffset.y, zoneOffset.z } };
		if (useRelativePosion)
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
			std::vector<CVector> vertices;
			std::vector<CUV> textureCordinates;

			buildFaces(landscape, zoneId, patchIndex, vertices, textureCordinates);

			gltf::Primitive primitive = { .attributes = { .position = 0, .texcoord0 = 1 } };
			gltf::Accessor position = { .bufferView = 0, .byteOffset = outputPosition.getPos(), .componentType = gltf::ComponentType::FLOAT, .count = vertices.size(), .type = gltf::AccessorType::VEC3 };
			gltf::Accessor textcoord0 = { .bufferView = 1, .byteOffset = outputTextureCordinate.getPos(), .componentType = gltf::ComponentType::FLOAT, .count = textureCordinates.size(), .type = gltf::AccessorType::VEC2 };
			primitive.attributes.position = asset.accessors.size();
			asset.accessors.push_back(position);
			primitive.attributes.texcoord0 = asset.accessors.size();
			asset.accessors.push_back(textcoord0);
			for (auto &texture : patch->Tiles)
			{
				auto tileId = texture.Tile[0];
				if (tileId != NL_TILE_ELM_LAYER_EMPTY)
				{
					if (!bankFilePath.empty())
					{
						if (tileId < asset.materials.size())
						{
							primitive.material = tileId;
						}
						else
						{
							nlerror("PatchTexture tileId not in tileset %i >= %i", tileId, asset.materials.size());
						}
					} else
					{
						const auto name = materialName(tileId);
						primitive.material = asset.materials.size();
						for( auto i = 0; i < asset.materials.size(); ++i )
						{
							if ( asset.materials[i].name == name )
							{
								primitive.material = i;
							}
						}
						if ( primitive.material == asset.materials.size())
						{
							asset.materials.push_back({ .name = name });
						}
					}
				}
			}
			mesh.primitives.push_back(primitive);

			// Add to the file
			for (auto &vertex : vertices)
			{
				vertex -= zoneOffset;
				// clampVertice(vertex);
				// validatePatchVertice(vertex, zone.getPatchScale(), zoneOffset);
				vertex.serial(outputPosition);
				if (firstVertex)
				{
					firstVertex = false;
					bbox.setCenter(vertex);
				}
				else
				{
					bbox.extend(vertex);
				}
			}

			for (auto &uv : textureCordinates)
			{
				// Serial the triangle
				uv.serial(outputTextureCordinate);
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
		asset.bufferViews.push_back({ .buffer = asset.buffers.size(), .byteLength = outputTextureCordinate.getPos() });
		asset.buffers.push_back({ .uri = textureCordinateFileName, .byteLength = outputTextureCordinate.getPos() });
		gltfWriter.write(asset);
		fclose(fp);
		outputPosition.close();
		outputTextureCordinate.close();

		return EXIT_SUCCESS;
	}
	catch (const Exception &e)
	{
		nlwarning("Error in writing zone file: %s", e.what());
		return EXIT_FAILURE;
	}
}
