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
#include <nel/3d/mesh.h>
#include <nel/3d/mesh_mrm.h>
#include <nel/3d/mesh_mrm_skinned.h>
#include <nel/3d/scene.h>
#include <nel/3d/register_3d.h>
#include <nel/misc/app_context.h>
#include <nel/misc/o_xml.h>
#include <nel/misc/i_xml.h>

using namespace NL3D;
using namespace NLMISC;
using namespace std;

bool processMesh(IShape *shape);

int main(int argc, char **argv)
{
	try
	{
		NLMISC::CApplicationContext myApplicationContext;
		NLMISC::CCmdArgs args;

		args.addAdditionalArg("input", "Input shape file");
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
		std::string indicesCordinateFileName = fileName + ".indices.bin";
		std::string indicesCordinateFilePath = outputDirectory + "/" + indicesCordinateFileName;

		registerSerial3d();
		CScene::registerBasics();

		CIFile inputFile(inputFilePath);
		CShapeStream shapeStream;
		shapeStream.serial(inputFile);
		inputFile.close();
		IShape *shape = shapeStream.getShapePointer();
		if (!processMesh(shape))
		{
			nlwarning("File not a CMesh");
			return EXIT_FAILURE;
		}
		COFile outputPosition;
		if (!outputPosition.open(positionFilePath, false, false, false))
		{
			nlwarning("Can't open the file for writing: %s", positionFilePath.c_str());
			return EXIT_FAILURE;
		}
		COFile outputIndicesCordinate;
		if (!outputIndicesCordinate.open(indicesCordinateFilePath, false, false, false))
		{
			nlwarning("Can't open the file for writing: %s", indicesCordinateFilePath.c_str());
			return EXIT_FAILURE;
		}

		// vector of triangle
		std::vector<CTriangle> faces;

		// Add to the file
		for (auto &face : faces)
		{
			// Serial the triangle
			face.V0.serial(outputPosition);
			face.V1.serial(outputPosition);
			face.V2.serial(outputPosition);
		}

		FILE *fp = nlfopen(outputFilePath, "w");
		if (fp == NULL)
		{
			nlwarning("Can't open the file for writing: %s", outputFilePath.c_str());
			return EXIT_FAILURE;
		}
		fprintf(fp, "{\n");
		fprintf(fp, "    \"asset\": { \"version\": \"2.0\" },\n");
		fprintf(fp, "    \"meshes\": [\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"primitives\": [\n");
		fprintf(fp, "				{\n");
		fprintf(fp, "					\"attributes\": { \"POSITION\": 0 }\n");
		fprintf(fp, "				},\n");
		fprintf(fp, "				\"indices\": 1\n");
		fprintf(fp, "			]\n");
		fprintf(fp, "        }\n");
		fprintf(fp, "    ],\n");
		fprintf(fp, "    \"accessors\": [\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"bufferView\": 0,\n");
		fprintf(fp, "			\"componentType\": 5126,\n");
		fprintf(fp, "			\"count\": %i,\n", triangleCount * 3);
		fprintf(fp, "			\"type\": \"VEC3\"\n");
		fprintf(fp, "        },\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"bufferView\": 1,\n");
		fprintf(fp, "			\"componentType\": 5126,\n");
		fprintf(fp, "			\"count\": %i,\n", textrueCordinateCount);
		fprintf(fp, "			\"max\": [1.0, 1.0],\n");
		fprintf(fp, "			\"min\": [1.0, 1.0],\n");
		fprintf(fp, "			\"type\": \"VEC2\"\n");
		fprintf(fp, "        }\n");
		fprintf(fp, "    ],\n");
		fprintf(fp, "    \"bufferViews\": [\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"buffer\": 0,\n");
		fprintf(fp, "			\"byteLength\": %i\n", outputPosition.getPos());
		fprintf(fp, "        },\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"buffer\": 1,\n");
		fprintf(fp, "			\"byteLength\": %i\n", outputIndicesCordinate.getPos());
		fprintf(fp, "        }\n");
		fprintf(fp, "    ],\n");
		fprintf(fp, "    \"buffers\": [\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"uri\": \"%s\",\n", positionFileName.c_str());
		fprintf(fp, "			\"byteLength\": %i\n", outputPosition.getPos());
		fprintf(fp, "        },\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"uri\": \"%s\",\n", indicesCordinateFileName.c_str());
		fprintf(fp, "			\"byteLength\": %i\n", outputIndicesCordinate.getPos());
		fprintf(fp, "        }\n");
		fprintf(fp, "    ]\n");
		fprintf(fp, "}\n");
		fclose(fp);
		outputPosition.close();

		return EXIT_SUCCESS;
	}
	catch (const Exception &e)
	{
		nlwarning("Error convertig shape file file: %s", e.what());
		return EXIT_FAILURE;
	}
}

bool processMesh(IShape *shape)
{
	CMesh *mesh = dynamic_cast<CMesh *>(shape);

	if (!mesh)
		return false;

	nlinfo("File is a CMesh");

	return false;
}