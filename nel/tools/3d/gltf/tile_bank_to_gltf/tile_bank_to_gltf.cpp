#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

#include <nel/misc/types_nl.h>
#include <nel/misc/file.h>
#include <nel/misc/common.h>
#include <nel/misc/cmd_args.h>
#include <nel/misc/bitmap.h>
#include <nel/3d/tile_bank.h>

#include <libgltf/gltf.h>

using namespace NL3D;
using namespace NLMISC;
using namespace std;

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

std::string materialName(const uint16 tileId)
{
	std::ostringstream name;

	name << "M_tile_id_" << tileId;

	return name.str();
}

int main(int argc, char **argv)
{
	try
	{
		NLMISC::CApplicationContext myApplicationContext;
		NLMISC::CCmdArgs args;

		args.addAdditionalArg("input", ".smallbank TileBank to load");
		args.addAdditionalArg("output", "Output gltf file");
		args.addArg("", "image-prefix", "path", "prefix to add for image uris");
		args.addArg("", "image-extension", "ext", "file extension to use for images");

		if (!args.parse(argc, argv))
		{
			args.displayHelp();
			return EXIT_FAILURE;
		}

		std::string bankFilePath = args.getAdditionalArg("input").front();
		std::string outputFilePath = args.getAdditionalArg("output").front();
		std::string imageUriPrefix = getLongArgFirstValue(args, "image-prefix");
		std::string imageFileExtension = getLongArgFirstValue(args, "image-extension");

		CIFile bankFile;
		if (!bankFile.open(bankFilePath))
		{
			nlwarning("Can't open the input file for reading: %s", bankFilePath.c_str());
			return EXIT_FAILURE;
		}
		std::vector<gltf::Image> images;
		std::vector<gltf::Texture> textures;
		std::vector<gltf::Material> materials;
		std::map<std::string, size_t> filenameToTextureIndex;
		std::map<uint16, size_t> tileIdToTexture;
		CTileBank tileBank;
		tileBank.serial(bankFile);
		nldebug("TileBank land count %i", tileBank.getLandCount());
		nldebug("TileBank tileSet count %i", tileBank.getTileSetCount());
		nldebug("TileBank tile count %i", tileBank.getTileCount());
		for (auto tileId = 0; tileId < tileBank.getTileCount(); ++tileId)
		{
			auto tile = tileBank.getTile(tileId);
			std::string imageUri = tile->getFileName(CTile::diffuse);
			if (tile->isFree())
			{
				nlinfo("Tile is free %i %s", tileId, imageUri.c_str());
				continue;
			}
			nldebug("Tile %d has diffuse texture %s", tileId, imageUri.c_str());
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
		gltf::Asset asset = {
			.materials = materials,
			.textures = textures,
			.images = images
		};

		FILE *fp = nlfopen(outputFilePath, "w");
		if (fp == NULL)
		{
			nlwarning("Can't open the output file for writing: %s", outputFilePath.c_str());
			return EXIT_FAILURE;
		}
		gltf::JsonWriter gltfWriter = { .file = fp };
		gltfWriter.write(asset);
		fclose(fp);

		return EXIT_SUCCESS;
	}
	catch (const Exception &e)
	{
		nlwarning("Error: %s", e.what());
		return EXIT_FAILURE;
	}
}
