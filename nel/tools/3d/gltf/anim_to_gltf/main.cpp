#include <iostream>
#include <vector>
#include <set>

#include <nel/misc/types_nl.h>
#include <nel/misc/common.h>
#include <nel/misc/cmd_args.h>
#include <nel/misc/file.h>
#include <nel/3d/animation.h>
#include <nel/3d/scene.h>
#include <nel/3d/register_3d.h>
#include <nel/misc/app_context.h>

#include <libgltf/gltf.h>

#include "common.h"

using namespace NL3D;
using namespace NLMISC;
using namespace std;

int main(int argc, char **argv)
{
	try
	{
		CApplicationContext myApplicationContext;
		CCmdArgs args;

		args.addAdditionalArg("input", "Input anim file");
		args.addAdditionalArg("output", "Output gltf file");

		if (!args.parse(argc, argv))
		{
			args.displayHelp();
			return EXIT_FAILURE;
		}

		std::string inputFilePath = args.getAdditionalArg("input").front();
		std::string outputFilePath = args.getAdditionalArg("output").front();
		std::string outputDirectory = CFile::getPath(outputFilePath);
		std::string baseFileName = CFile::getFilenameWithoutExtension(outputFilePath);

		registerSerial3d();
		CScene::registerBasics();

		CIFile inputFile(inputFilePath);
		CAnimation input;
		input.serial(inputFile);
		inputFile.close();

		nldebug("Time Begin %f End %f", input.getBeginTime(), input.getEndTime());
		std::set<std::string> trackNames;
		input.getTrackNames(trackNames);
		for (auto &name : trackNames)
		{
			auto trackId = input.getIdTrackByName(name);
			nldebug("track name %s id %i", name.c_str(), trackId);
			if (trackId != CAnimation::NotFound)
			{
				auto track = input.getTrack(trackId);
				nldebug("track is %s", track->getClassName().c_str());
			}
		}

		gltf::Asset asset = {
			.animations = {}
		};

		FILE *fp = nlfopen(outputFilePath, "w");
		if (fp == NULL)
		{
			nlwarning("Can't open the file for writing: %s", outputFilePath.c_str());
			return EXIT_FAILURE;
		}
		gltf::JsonWriter gltfWriter = { .file = fp };
		gltfWriter.write(asset);
		fclose(fp);

		return EXIT_SUCCESS;
	}
	catch (const Exception &e)
	{
		nlwarning("Error convertig shape file file: %s", e.what());
		return EXIT_FAILURE;
	}
}

std::string getLongArgFirstValue(const CCmdArgs &args, const std::string &argName)
{
	std::string firstValue;
	const auto values = args.getLongArg(argName);
	if (!values.empty())
	{
		firstValue = values.front();
	}
	return firstValue;
}
