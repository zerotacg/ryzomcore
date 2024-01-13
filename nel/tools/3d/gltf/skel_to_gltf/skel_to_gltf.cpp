#include <iostream>
#include <vector>

#include <nel/misc/types_nl.h>
#include <nel/misc/common.h>
#include <nel/misc/cmd_args.h>
#include <nel/misc/file.h>
#include <nel/3d/mesh.h>
#include <nel/3d/scene.h>
#include <nel/3d/skeleton_shape.h>
#include <nel/3d/register_3d.h>
#include <nel/misc/app_context.h>

#include <libgltf/gltf.h>

#include "skel_to_gltf.h"

using namespace NL3D;
using namespace NLMISC;
using namespace std;

int main(int argc, char **argv)
{
	try
	{
		CApplicationContext myApplicationContext;
		CCmdArgs args;

		args.addAdditionalArg("input", "Input skel file");
		args.addAdditionalArg("output", "Output gltf file");

		if (!args.parse(argc, argv))
		{
			args.displayHelp();
			return EXIT_FAILURE;
		}

		std::string inputFilePath = args.getAdditionalArg("input").front();
		std::string outputFilePath = args.getAdditionalArg("output").front();
		std::string outputDirectory = CFile::getPath(outputFilePath);
		std::string fileName = CFile::getFilenameWithoutExtension(outputFilePath);
		std::string positionFileName = fileName + ".position.bin";
		std::string positionFilePath = outputDirectory + "/" + positionFileName;
		std::string indicesFileName = fileName + ".indices.bin";
		std::string indicesFilePath = outputDirectory + "/" + indicesFileName;
		std::string normalsFileName = fileName + ".normal.bin";
		std::string normalsFilePath = outputDirectory + "/" + normalsFileName;
		std::string textureCoordinatesFileName = fileName + ".texcoord_0.bin";
		std::string textureCoordinatesFilePath = outputDirectory + "/" + textureCoordinatesFileName;

		registerSerial3d();
		CScene::registerBasics();

		CIFile inputFile(inputFilePath);
		CShapeStream shapeStream;
		shapeStream.serial(inputFile);
		inputFile.close();
		IShape *shape = shapeStream.getShapePointer();
		nlinfo("File is a %s", shape->getClassName().c_str());
		auto *skeleton = dynamic_cast<CSkeletonShape *>(shape);

		if (!skeleton)
		{
			nlwarning("File not a CSkeletonShape");
			return EXIT_FAILURE;
		}

		gltf::Skin skin;
		std::vector<gltf::Node> nodes;
		vector<CBoneBase> bones;
		skeleton->retrieve(bones);
		for (auto &bone : bones)
		{
			nlinfo("Bone %s parent %i", bone.Name.c_str(), bone.FatherId);
			auto &pos = bone.DefaultPos.getDefaultValue();
			auto &quat = bone.DefaultRotQuat.getDefaultValue();
			skin.joints.push_back(nodes.size());
			nodes.push_back({ .name = bone.Name,
			    .translation = { pos.x, pos.y, pos.z },
			    .rotation = { quat.x, quat.y, quat.z, quat.w }
			});
			auto parentId = bone.FatherId;
			if (parentId != -1)
			{
				auto &parent = bones[parentId];
				nlinfo("Parent %s", parent.Name.c_str());
			}
		}
		for (auto i = 0; i < bones.size(); ++i)
		{
			auto &bone = bones[i];
			auto parentId = bone.FatherId;
			if (parentId != -1)
			{
				nodes[parentId].children.push_back(i);
			}
		}
		skin.skeleton = nodes.size();
		nodes.push_back({ .mesh = 0, .skin = 0, .children = { 0 } });

		FILE *fp = nlfopen(outputFilePath, "w");
		if (fp == NULL)
		{
			nlwarning("Can't open the file for writing: %s", outputFilePath.c_str());
			return EXIT_FAILURE;
		}
		gltf::JsonWriter gltfWriter = { .file = fp };

		gltf::Asset asset = {
			.nodes = nodes,
			.skins = { skin },
			.scenes = { { .nodes = { nodes.size() - 1 } } }
		};

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
