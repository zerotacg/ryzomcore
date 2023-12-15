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

#include "../common/gltf.h"

using namespace NL3D;
using namespace NLMISC;
using namespace std;

struct MeshPart
{
	vector<uint32> indices;
	vector<string> textures;
};

bool processMesh(IShape *shape, vector<CVector> &vertices, vector<CVector> &normals, vector<CUV> &textureCoordinates, vector<MeshPart> &parts);

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

		args.addAdditionalArg("input", "Input shape file");
		args.addAdditionalArg("output", "Output gltf file");
		args.addArg("", "imageUriPrefix", "path", "prefix to add for image uris");
		args.addArg("", "imageFileExtension", "ext", "file extension to use for images");
		args.addArg("", "imageFileLowerCase", "", "convert filename to lower case");
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
		std::string imageUriPrefix = getLongArgFirstValue(args, "imageUriPrefix");
		std::string imageFileExtension = getLongArgFirstValue(args, "imageFileExtension");
		bool imageFileLowerCase = args.haveLongArg("imageFileLowerCase");

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
		std::vector<MeshPart> parts;
		nlinfo("File is a %s", shape->getClassName().c_str());

		if (!processMesh(shape, vertices, normals, textureCoordinates, parts))
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
		gltf::JsonWriter gltfFile = { .file = fp };
		fprintf(fp, "{\n");
		fprintf(fp, "    \"asset\": { \"version\": \"2.0\" },\n");
		fprintf(fp, "    \"meshes\": [\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "            \"primitives\": [\n");
		const uint32 firstIndicesAccessor = 3;
		std::vector<gltf::Material> materials;
		std::vector<gltf::Texture> textures;
		std::vector<gltf::Image> images;
		for (auto i = 0; i < parts.size(); ++i)
		{
			auto part = parts[i];
			if (i > 0)
			{
				fprintf(fp, "                ,{\n");
			}
			else
			{
				fprintf(fp, "                {\n");
			}
			fprintf(fp, "                    \"attributes\": { \"POSITION\": 0, \"NORMAL\": 1, \"TEXCOORD_0\": 2 }\n");
			fprintf(fp, "                   ,\"indices\": %i\n", firstIndicesAccessor + i);
			if (!part.textures.empty())
			{
				fprintf(fp, "                   ,\"material\": %lu\n", materials.size());
				auto textureFile = part.textures.front();
				if (imageFileLowerCase)
				{
					textureFile = toLower(textureFile);
				}
				if (!imageFileExtension.empty())
				{
					textureFile = CFile::getFilenameWithoutExtension(textureFile);
					textureFile += ".";
					textureFile += imageFileExtension;
				}
				std::string imageUri = imageUriPrefix + textureFile;

				gltf::Texture texture = { images.size() };
				gltf::Material material = { { textures.size() } };
				for (auto j = 0; j < images.size(); ++j)
				{
					if (images[j].uri == imageUri)
					{
						texture.source = j;
						material.pbrMetallicRoughness.baseColorTexture.index = j;
					}
				}
				if (texture.source == images.size())
				{
					images.push_back(gltf::Image { imageUri });
					textures.push_back(texture);
				}
				materials.push_back(material);
			}
			fprintf(fp, "                }\n");
		}
		fprintf(fp, "            ]\n");
		fprintf(fp, "        }\n");
		fprintf(fp, "    ]\n");
		if (!materials.empty())
		{
			fprintf(fp, "   ,\"materials\": ");
			gltfFile.write(materials);
			fprintf(fp, "\n");
		}
		if (!textures.empty())
		{
			fprintf(fp, "   ,\"textures\": ");
			gltfFile.write(textures);
			fprintf(fp, "\n");
		}
		if (!images.empty())
		{
			fprintf(fp, "   ,\"images\": ");
			gltfFile.write(images);
			fprintf(fp, "\n");
		}
		fprintf(fp, "   ,\"accessors\": [\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "            \"bufferView\": 0,\n");
		fprintf(fp, "            \"componentType\": 5126,\n");
		fprintf(fp, "            \"count\": %lu,\n", vertices.size());
		fprintf(fp, "            \"max\": [1.0, 1.0, 1.0],\n");
		fprintf(fp, "            \"min\": [-1.0, -1.0, -1.0],\n");
		fprintf(fp, "            \"type\": \"VEC3\"\n");
		fprintf(fp, "        },\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "            \"bufferView\": 1,\n");
		fprintf(fp, "            \"componentType\": 5126,\n");
		fprintf(fp, "            \"count\": %lu,\n", normals.size());
		fprintf(fp, "            \"max\": [1.0, 1.0, 1.0],\n");
		fprintf(fp, "            \"min\": [-1.0, -1.0, -1.0],\n");
		fprintf(fp, "            \"type\": \"VEC3\"\n");
		fprintf(fp, "        },\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "            \"bufferView\": 2,\n");
		fprintf(fp, "            \"componentType\": 5126,\n");
		fprintf(fp, "            \"count\": %lu,\n", textureCoordinates.size());
		fprintf(fp, "            \"max\": [1.0, 1.0],\n");
		fprintf(fp, "            \"min\": [0.0, 0.0],\n");
		fprintf(fp, "            \"type\": \"VEC2\"\n");
		fprintf(fp, "        }\n");
		for (auto &part : parts)
		{
			auto indices = part.indices;
			fprintf(fp, ",");
			gltf::Accessor accessor = { 3, outputIndices.getPos(), gltf::UNSIGNED_INT, indices.size(), gltf::SCALAR };
			gltfFile.write(accessor);
			fprintf(fp, "\n");
			for (auto &element : indices)
			{
				outputIndices.serial(element);
			}
		}

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

uint32 getIndexAt(const CIndexBufferRead &buffer, const int index)
{
	if (buffer.getFormat() == CIndexBuffer::Indices32)
	{
		const auto *indexPointer = static_cast<const uint32 *>(buffer.getPtr());
		return *(indexPointer + index);
	}
	else
	{
		const auto *indexPointer = static_cast<const uint16 *>(buffer.getPtr());
		return *(indexPointer + index);
	}
}

bool processMesh(IShape *shape, vector<CVector> &vertices, vector<CVector> &normals, vector<CUV> &textureCoordinates, vector<MeshPart> &parts)
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

	for (auto i = 0; i < vertexBuffer.getNumVertices(); ++i)
	{
		vertices.push_back(*vba.getVertexCoordPointer(i));
		normals.push_back(*vba.getNormalCoordPointer(i));
		textureCoordinates.push_back(*vba.getTexCoordPointer(i));
	}

	for (auto renderPass = 0; renderPass < mesh->getNbRdrPass(lodId); ++renderPass)
	{
		auto indexBuffer = mesh->getRdrPassPrimitiveBlock(lodId, renderPass);
		auto materialIndex = mesh->getRdrPassMaterial(lodId, renderPass);
		nlinfo("RenderPasss %i Elements %i Material %i", renderPass, indexBuffer.getNumIndexes(), materialIndex);
		auto material = mesh->getMaterial(materialIndex);
		if (material.getBlend())
		{
			nlinfo("Material Blend");
		}
		vector<string> textures;
		for (auto textureIndex = 0; textureIndex < IDRV_MAT_MAXTEXTURES; ++textureIndex)
		{
			if (material.texturePresent(textureIndex))
			{
				nlinfo("Texture at index %i is %s", textureIndex, material.getTexture(textureIndex)->getClassName().c_str());
				auto textureFile = dynamic_cast<CTextureFile *>(material.getTexture(textureIndex));
				if (textureFile)
				{
					nlinfo("CTextureFile %s", textureFile->getFileName().c_str());
					textures.push_back(textureFile->getFileName());
				}
				else
				{
					nlwarning("Texture at index %i is not a CTextureFile", textureIndex);
				}
			}
		}
		CIndexBufferRead iba;
		indexBuffer.lock(iba);
		vector<uint32> indices;
		switch (indexBuffer.getFormat())
		{
		case CIndexBuffer::Indices16:
			nlinfo("IndexBuffer Format: Indices16");
			break;
		case CIndexBuffer::Indices32:
			nlinfo("IndexBuffer Format: Indices32");
			break;
		case CIndexBuffer::IndicesUnknownFormat:
			nlinfo("IndexBuffer Format: IndicesUnknownFormat");
			break;
		}

		for (auto i = 0; i < indexBuffer.getNumIndexes(); ++i)
		{
			uint32 idx = getIndexAt(iba, i);
			if (idx != -1)
			{
				indices.push_back(idx);
			}
		}
		nldebug("index min %i max %i", *min_element(indices.begin(), indices.end()), *max_element(indices.begin(), indices.end()));
		MeshPart part = { indices, textures };
		parts.push_back(part);
	}

	return true;
}
