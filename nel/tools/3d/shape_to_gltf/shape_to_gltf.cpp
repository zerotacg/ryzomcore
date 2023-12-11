#include <iostream>
#include <nel/misc/types_nl.h>
#include <nel/misc/file.h>
#include <nel/misc/o_xml.h>
#include <nel/misc/common.h>
#include <nel/misc/cmd_args.h>
#include <nel/misc/bitmap.h>
#include <nel/3d/zone.h>
#include <nel/3d/landscape.h>
#include <nel/3d/texture_file.h>
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

bool processMesh(IShape *shape, vector<CVector> &vertices, vector<CVector> &normals, vector<CUV> &textureCoordinates, vector<uint32> &indices);
const CIndexBuffer *getRdrPassPrimitiveBlock(const CMeshGeom *mesh, uint lodId, uint renderPass);

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
		std::vector<CVector> vertices;
		std::vector<CVector> normals;
		std::vector<CUV> textureCoordinates;
		std::vector<uint32> indices;

		if (!processMesh(shape, vertices, normals, textureCoordinates, indices))
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
		COFile outputIndices;
		if (!outputIndices.open(indicesFilePath, false, false, false))
		{
			nlwarning("Can't open the file for writing: %s", indicesFilePath.c_str());
			return EXIT_FAILURE;
		}

		COFile outputNormals;
		if (!outputNormals.open(normalsFilePath, false, false, false))
		{
			nlwarning("Can't open the file for writing: %s", normalsFilePath.c_str());
			return EXIT_FAILURE;
		}
		COFile outputTextureCoordinates;
		if (!outputTextureCoordinates.open(textureCoordinatesFilePath, false, false, false))
		{
			nlwarning("Can't open the file for writing: %s", textureCoordinatesFilePath.c_str());
			return EXIT_FAILURE;
		}

		for (auto &element : indices)
		{
			outputIndices.serial(element);
		}

		for (auto &element : vertices)
		{
			element.serial(outputPosition);
		}

		for (auto &element : normals)
		{
			element.serial(outputNormals);
		}

		for (auto &element : textureCoordinates)
		{
			element.serial(outputTextureCoordinates);
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
		fprintf(fp, "					\"attributes\": { \"POSITION\": 0, \"NORMAL\": 1, \"TEXCOORD_0\": 2 },\n");
		fprintf(fp, "					\"indices\": 3\n");
		fprintf(fp, "				}\n");
		fprintf(fp, "			]\n");
		fprintf(fp, "        }\n");
		fprintf(fp, "    ],\n");
		fprintf(fp, "    \"accessors\": [\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"bufferView\": 0,\n");
		fprintf(fp, "			\"componentType\": 5126,\n");
		fprintf(fp, "			\"count\": %lu,\n", vertices.size());
		fprintf(fp, "			\"max\": [1.0, 1.0, 1.0],\n");
		fprintf(fp, "			\"min\": [-1.0, -1.0, -1.0],\n");
		fprintf(fp, "			\"type\": \"VEC3\"\n");
		fprintf(fp, "        },\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"bufferView\": 1,\n");
		fprintf(fp, "			\"componentType\": 5126,\n");
		fprintf(fp, "			\"count\": %lu,\n", normals.size());
		fprintf(fp, "			\"max\": [1.0, 1.0, 1.0],\n");
		fprintf(fp, "			\"min\": [-1.0, -1.0, -1.0],\n");
		fprintf(fp, "			\"type\": \"VEC3\"\n");
		fprintf(fp, "        },\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"bufferView\": 2,\n");
		fprintf(fp, "			\"componentType\": 5126,\n");
		fprintf(fp, "			\"count\": %lu,\n", textureCoordinates.size());
		fprintf(fp, "			\"max\": [1.0, 1.0],\n");
		fprintf(fp, "			\"min\": [0.0, 0.0],\n");
		fprintf(fp, "			\"type\": \"VEC2\"\n");
		fprintf(fp, "        },\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"bufferView\": 3,\n");
		fprintf(fp, "			\"componentType\": 5125,\n");
		fprintf(fp, "			\"count\": %lu,\n", indices.size());
		fprintf(fp, "			\"max\": %i,\n", *max_element(indices.begin(), indices.end()));
		fprintf(fp, "			\"min\": %i,\n", *min_element(indices.begin(), indices.end()));
		fprintf(fp, "			\"type\": \"SCALAR\"\n");
		fprintf(fp, "        }\n");
		fprintf(fp, "    ],\n");
		fprintf(fp, "    \"bufferViews\": [\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"buffer\": 0,\n");
		fprintf(fp, "			\"byteLength\": %i\n", outputPosition.getPos());
		fprintf(fp, "        },\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"buffer\": 1,\n");
		fprintf(fp, "			\"byteLength\": %i\n", outputNormals.getPos());
		fprintf(fp, "        },\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"buffer\": 2,\n");
		fprintf(fp, "			\"byteLength\": %i\n", outputTextureCoordinates.getPos());
		fprintf(fp, "        },\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"buffer\": 3,\n");
		fprintf(fp, "			\"byteLength\": %i\n", outputIndices.getPos());
		fprintf(fp, "        }\n");
		fprintf(fp, "    ],\n");
		fprintf(fp, "    \"buffers\": [\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"uri\": \"%s\",\n", positionFileName.c_str());
		fprintf(fp, "			\"byteLength\": %i\n", outputPosition.getPos());
		fprintf(fp, "        },\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"uri\": \"%s\",\n", normalsFileName.c_str());
		fprintf(fp, "			\"byteLength\": %i\n", outputNormals.getPos());
		fprintf(fp, "        },\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"uri\": \"%s\",\n", textureCoordinatesFileName.c_str());
		fprintf(fp, "			\"byteLength\": %i\n", outputTextureCoordinates.getPos());
		fprintf(fp, "        },\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"uri\": \"%s\",\n", indicesFileName.c_str());
		fprintf(fp, "			\"byteLength\": %i\n", outputIndices.getPos());
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

bool processMesh(IShape *shape, vector<CVector> &vertices, vector<CVector> &normals, vector<CUV> &textureCoordinates, vector<uint32> &indices)
{
	auto *mesh = dynamic_cast<CMesh *>(shape);

	if (!mesh)
		return false;

	nlinfo("File is a CMesh");

	CVertexBuffer vertexBuffer = mesh->getVertexBuffer();
	CVertexBufferRead vba;
	vertexBuffer.lock(vba);
	const uint lodId = 0;
	const auto lodCount = mesh->getNbMatrixBlock();
	nlinfo("LodCount %i", lodCount);

	for (auto renderPass = 0; renderPass < mesh->getNbRdrPass(lodId); ++renderPass)
	{
		auto indexBuffer = mesh->getRdrPassPrimitiveBlock(lodId, renderPass);
		auto materialIndex = mesh->getRdrPassMaterial(lodId, renderPass);
		nlinfo("RenderPasss %i Material %i", renderPass, materialIndex);
		auto material = mesh->getMaterial(materialIndex);
		for (auto textureIndex = 0; textureIndex < IDRV_MAT_MAXTEXTURES; ++textureIndex)
		{
			if (material.texturePresent(textureIndex))
			{
				auto textureFile = dynamic_cast<CTextureFile *>(material.getTexture(textureIndex));
				if (textureFile)
				{
					nlinfo("CTextureFile %s", textureFile->getFileName().c_str());
				}
				else
				{
					nlwarning("Texture at index %i is not a CTextureFile", textureIndex);
				}
			}
		}
		CIndexBufferRead iba;
		indexBuffer.lock(iba);
		if (iba.getFormat() == CIndexBuffer::Indices32)
		{
			const auto *triPtr = static_cast<const uint32 *>(iba.getPtr());
			for (auto i = 0; i < indexBuffer.getNumIndexes(); ++i)
			{
				uint32 idx = *triPtr;
				if (idx != -1)
				{
					indices.push_back(idx);
				}
				triPtr++;
			}
		}
		else
		{
			const auto *triPtr = static_cast<const uint16 *>(iba.getPtr());
			for (auto j = 0; j < indexBuffer.getNumIndexes(); ++j)
			{
				uint32 idx = *triPtr;
				if (idx != -1)
				{
					indices.push_back(idx);
				}
				triPtr++;
			}
		}
		for (auto j = 0; j < indexBuffer.getNumIndexes(); ++j)
		{
			vertices.push_back(*vba.getVertexCoordPointer(j));
			normals.push_back(*vba.getNormalCoordPointer(j));
			textureCoordinates.push_back(*vba.getTexCoordPointer(j));
		}
	}

	return true;
}

const CIndexBuffer *getRdrPassPrimitiveBlock(const CMeshGeom *mesh, uint lodId, uint renderPass)
{
	return &(mesh->getRdrPassPrimitiveBlock(lodId, renderPass));
}
