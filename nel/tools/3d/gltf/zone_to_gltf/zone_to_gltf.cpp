#include <iostream>
#include <nel/misc/types_nl.h>
#include <nel/misc/file.h>
#include <nel/misc/o_xml.h>
#include <nel/misc/common.h>
#include <nel/misc/cmd_args.h>
#include <nel/misc/bitmap.h>
#include <nel/3d/zone.h>
#include <nel/3d/landscape.h>
#include <nel/ligo/zone_region.h>
#include <vector>

#include <libgltf/gltf.h>

using namespace NL3D;
using namespace NLMISC;
using namespace NLLIGO;
using namespace std;

void buildFaces(CLandscape &landscape, sint zoneId, sint patch, std::vector<CTriangle> &faces, std::vector<CUV> &textureCordinates)
{
	faces.clear();

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
			CTriangle f;
			// CUV a(x*OOS, y*OOT), b(x*OOS, (y+1)*OOT), c((x+1)*OOS, (y+1)*OOT), d((x+1)*OOS, y*OOT);
			CUV a(0, 0), b(0, 1), c(1, 1), d(1, 0);

			f.V0 = pa->computeContinousVertex(x * OOS, y * OOT);
			textureCordinates.push_back(a);
			f.V1 = pa->computeContinousVertex(x * OOS, (y + 1) * OOT);
			textureCordinates.push_back(b);
			f.V2 = pa->computeContinousVertex((x + 1) * OOS, (y + 1) * OOT);
			textureCordinates.push_back(c);
			faces.push_back(f);
			f.V0 = pa->computeContinousVertex(x * OOS, y * OOT);
			textureCordinates.push_back(a);
			f.V1 = pa->computeContinousVertex((x + 1) * OOS, (y + 1) * OOT);
			textureCordinates.push_back(c);
			f.V2 = pa->computeContinousVertex((x + 1) * OOS, y * OOT);
			textureCordinates.push_back(d);
			faces.push_back(f);
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
		CZone zone;
		zone.serial(zoneFile);
		landscape.setNoiseMode(false);
		landscape.addZone(zone);
		zoneFile.close();
		uint32 triangleCount = 0;
		uint32 textrueCordinateCount = 0;
		COFile outputPosition;
		std::vector<gltf::Image> images;
		try
		{
			CIFile bankFile(bankFilePath);
			auto &tileBank = landscape.TileBank;
			tileBank.serial(bankFile);
			nldebug("TileBank land count %i", tileBank.getLandCount());
			nldebug("TileBank tileSet count %i", tileBank.getTileSetCount());
			nldebug("TileBank tile count %i", tileBank.getTileCount());
			for (auto i = 0; i < tileBank.getLandCount(); ++i)
			{
				nldebug("TileBank land %i '%s'", i, tileBank.getLand(i)->getName().c_str());
			}
			for (auto i = 0; i < tileBank.getTileSetCount(); ++i)
			{
				nldebug("TileBank tileSet %i '%s'", i, tileBank.getTileSet(i)->getName().c_str());
			}
			for (auto tileId = 0; tileId < tileBank.getTileCount(); ++tileId)
			{
				auto tile = tileBank.getTile(tileId);
				nldebug("TileBank tile %i diffuse: '%s' additive: '%s' alpha: '%s'", tileId, tile->getFileName(CTile::diffuse).c_str(), tile->getFileName(CTile::additive).c_str(), tile->getFileName(CTile::alpha).c_str());
				std::string imageUri = tile->getFileName(CTile::diffuse);
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

		CVector positionOffset = CVector::Null;
		if (useRelativePosion)
		{
			positionOffset = -zone.getPatchBias();
		}
		gltf::Mesh mesh;
		gltf::Asset asset = {
			.images = images
		};
		for (size_t i = 0; i < asset.images.size(); ++i)
		{
			asset.textures.push_back({ .source = i });
		}
		for (sint patchIndex = 0; patchIndex < zone.getNumPatchs(); patchIndex++)
		{
			const CPatch *patch = static_cast<const CZone &>(zone).getPatch(patchIndex);
			// vector of triangle
			std::vector<CTriangle> faces;
			std::vector<CUV> textureCordinates;

			buildFaces(landscape, zone.getZoneId(), patchIndex, faces, textureCordinates);

			gltf::Primitive primitive = { .attributes = { .position = 0, .texcoord0 = 1 } };
			gltf::Accessor position = { .bufferView = 0, .byteOffset = outputPosition.getPos(), .componentType = gltf::ComponentType::FLOAT, .count = faces.size() * 3, .type = gltf::AccessorType::VEC3 };
			gltf::Accessor textcoord0 = { .bufferView = 1, .byteOffset = outputTextureCordinate.getPos(), .componentType = gltf::ComponentType::FLOAT, .count = textureCordinates.size(), .type = gltf::AccessorType::VEC2 };
			primitive.attributes.position = asset.accessors.size();
			asset.accessors.push_back(position);
			primitive.attributes.texcoord0 = asset.accessors.size();
			asset.accessors.push_back(textcoord0);
			auto &textures = patch->Tiles;
			auto &tileBank = landscape.TileBank;
			for (auto &texture : textures)
			{
				auto tileId = texture.Tile[0];
				if (tileId != NL_TILE_ELM_LAYER_EMPTY)
				{
					if (tileBank.getTileCount() > tileId)
					{
						auto tile = tileBank.getTile(texture.Tile[0]);
						std::string diffuseTexture = tile->getFileName(CTile::diffuse);
						std::replace(diffuseTexture.begin(), diffuseTexture.end(), '\\', '/');
						nldebug("PatchTexture %i diffuse: '%s' additive: '%s' alpha: '%s'", patchIndex, diffuseTexture.c_str(), tile->getFileName(CTile::additive).c_str(), tile->getFileName(CTile::alpha).c_str());
						primitive.material = asset.materials.size();
						primitive.hasMaterial = true;
						for (auto i = 0; i < asset.materials.size(); ++i)
						{
							if (asset.materials[i].name == diffuseTexture)
							{
								primitive.material = i;
								break;
							}
						}
						if (primitive.material == asset.materials.size())
						{
							asset.materials.push_back({ .name = diffuseTexture, .pbrMetallicRoughness = { tileId }, .hasPbrMetallicRoughness = true });
						}
					}
					else
					{
						nlerror("PatchTexture tileId not in tileset %i >= %i", tileId, landscape.TileBank.getTileCount());
					}
				}
			}
			mesh.primitives.push_back(primitive);

			// Add to the file
			for (auto &face : faces)
			{
				face.V0 += positionOffset;
				face.V1 += positionOffset;
				face.V2 += positionOffset;
				// Serial the triangle
				face.V0.serial(outputPosition);
				face.V1.serial(outputPosition);
				face.V2.serial(outputPosition);
				if (firstVertex)
				{
					firstVertex = false;
					bbox.setCenter(face.V0);
				}
				else
				{
					bbox.extend(face.V0);
				}
				bbox.extend(face.V1);
				bbox.extend(face.V2);
			}

			for (auto &uv : textureCordinates)
			{
				// Serial the triangle
				uv.serial(outputTextureCordinate);
			}

			triangleCount += faces.size();
			textrueCordinateCount += textureCordinates.size();
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
