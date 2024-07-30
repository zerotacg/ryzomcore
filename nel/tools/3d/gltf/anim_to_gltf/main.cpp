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

#include <common.h>
#include <mapper/TrackMapper.h>

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
		std::string creatureSheetsFilePath = args.getAdditionalArg("creature-sheets").front();
		std::string outputDirectory = CFile::getPath(outputFilePath);
		std::string baseFileName = CFile::getFilenameWithoutExtension(outputFilePath);
		std::string dataFileName = baseFileName + ".bin";
		std::string dataFilePath = outputDirectory + "/" + dataFileName;

		registerSerial3d();
		CScene::registerBasics();

		CIFile inputFile(inputFilePath);
		CAnimation input;
		input.serial(inputFile);
		inputFile.close();

		COFile outputData;
		if (!outputData.open(dataFilePath, false, false, false))
		{
			nlwarning("Can't open the file for writing: %s", dataFilePath.c_str());
			return EXIT_FAILURE;
		}

		nlinfo("Time Begin %f End %f", input.getBeginTime(), input.getEndTime());
		std::set<std::string> trackNames;
		std::vector<gltf::Channel> channels;
		std::vector<gltf::Sampler> samplers;
		std::vector<gltf::Accessor> accessors;
		input.getTrackNames(trackNames);
		for (auto &name : trackNames)
		{
			auto trackId = input.getIdTrackByName(name);
			nlinfo("track name %s id %i", name.c_str(), trackId);
			if (trackId != CAnimation::NotFound)
			{
				auto track = input.getTrack(trackId);
				nlinfo("track is %s", track->getClassName().c_str());
				ChannelData data;
				TrackMapper::map(track, data);
				auto path = gltf::ChannelTargetPath::WEIGHTS;
				if (endsWith(name, "pos"))
				{
					path = gltf::ChannelTargetPath::TRANSLATION;
				}
				else if (endsWith(name, "rotquat"))
				{
					path = gltf::ChannelTargetPath::ROTATION;
				}
				else
				{
					nlwarning("Can't determine channel target for track: %s", name.c_str());
				}
				if (path != gltf::ChannelTargetPath::WEIGHTS)
				{
					auto input = accessors.size();
					accessors.push_back({ .bufferView = 0,
					    .byteOffset = outputData.getPos(),
					    .componentType = gltf::ComponentType::FLOAT,
					    .count = data.time.size(),
					    .type = gltf::AccessorType::SCALAR });
					for (auto value : data.time)
					{
						outputData.serial(value);
					}
					auto output = accessors.size();
					if (data.type == gltf::AccessorType::VEC3)
					{
						accessors.push_back({ .bufferView = 0,
						    .byteOffset = outputData.getPos(),
						    .componentType = gltf::ComponentType::FLOAT,
						    .count = data.vector.size(),
						    .type = data.type });
						for (auto value : data.vector)
						{
							outputData.serial(value);
						}
					}
					if (data.type == gltf::AccessorType::VEC4)
					{
						accessors.push_back({ .bufferView = 0,
						    .byteOffset = outputData.getPos(),
						    .componentType = gltf::ComponentType::FLOAT,
						    .count = data.quaternion.size(),
						    .type = data.type });
						for (auto value : data.quaternion)
						{
							outputData.serial(value);
						}
					}
					channels.push_back({ .sampler = samplers.size(),
					    .target = {
					        .node = 0,
					        .path = path,
					        .extras = name
					    } });
					samplers.push_back({ .input = input,
					    .interpolation = data.interpolation,
					    .output = output });
				}
			}
		}

		gltf::Asset asset = {
			.animations = {
			    { .name = baseFileName,
			        .channels = channels,
			        .samplers = samplers } },
			.accessors = accessors,
			.bufferViews = { { .buffer = 0, .byteLength = outputData.getPos() } },
			.buffers = { { .uri = dataFileName, .byteLength = outputData.getPos() } }
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

		outputData.close();

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
