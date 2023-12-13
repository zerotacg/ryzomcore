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
namespace gltf {
enum ComponentType : uint32
{
	SIGNED_BYTE = 5120,
	UNSIGNED_BYTE = 5121,
	SIGNED_SHORT = 5122,
	UNSIGNED_SHORT = 5123,
	UNSIGNED_INT = 5125,
	FLOAT = 5126
};
enum AccessorType
{
	SCALAR = 0,
	VEC2,
	VEC3,
	VEC4,
	MAT2,
	MAT3,
	MAT4
};
const char *AccessorTypeNames[] = { "SCALAR", "VEC2", "VEC3", "VEC4", "MAT2", "MAT3", "MAT4" };

struct Accessor
{
	uint32 bufferView;
	sint32 byteOffset;
	ComponentType componentType;
	size_t count;
	AccessorType type;
};

struct TextureInfo
{
	uint32 index;
};
struct NormalTextureInfo
{
	uint32 index;
	float scale;
};
struct OcclusionTextureInfo
{
	uint32 index;
	float strength;
};

struct MetallicRoughness
{
	float baseColorFactor[4];
	TextureInfo baseColorTexture;
	float metallicFactor;
	float roughnessFactor;
	TextureInfo metallicRoughnessTexture;
};

struct Material
{
	MetallicRoughness pbrMetallicRoughness;
	NormalTextureInfo normalTexture;
	OcclusionTextureInfo occlusionTexture;
	TextureInfo emissiveTexture;
	float emissiveFactor[3];
	string alphaMode;
	float alphaCutoff;
	bool doubleSided;
};

template <class T, class Allocator>
void write(FILE *file, std::vector<T, Allocator> &cont)
{
	typedef typename T::value_type __value_type;
	typedef typename T::iterator __iterator;

	fprintf(file, "[");
	auto len = cont.size();

	__iterator it = cont.begin();
	for (auto i = 0; i < len; i++, it++)
	{
		if (i > 0)
		{
			fprintf(file, ",");
		}
		write(const_cast<__value_type &>(*it));
	}
	fprintf(file, "]");
}

void write(FILE *file, const Accessor &object)
{
	fprintf(file, R"({ "bufferView": %i, "byteOffset": %i, "componentType": %i, "count": %lu, "type": "%s" })", object.bufferView, object.byteOffset, object.componentType, object.count, AccessorTypeNames[object.type]);
}
}

struct MeshPart
{
	vector<uint32> indices;
	vector<string> textures;
};

bool processMesh(IShape *shape, vector<CVector> &vertices, vector<CVector> &normals, vector<CUV> &textureCoordinates, vector<MeshPart> &parts);

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
		fprintf(fp, "{\n");
		fprintf(fp, "    \"asset\": { \"version\": \"2.0\" },\n");
		fprintf(fp, "    \"meshes\": [\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "            \"primitives\": [\n");
		const uint32 firstIndicesAccessor = 3;
		for (auto i = 0; i < parts.size(); ++i)
		{
			if (i > 0)
			{
				fprintf(fp, "                ,{\n");
			}
			else
			{
				fprintf(fp, "                {\n");
			}
			fprintf(fp, "                    \"attributes\": { \"POSITION\": 0, \"NORMAL\": 1, \"TEXCOORD_0\": 2 },\n");
			fprintf(fp, "                    \"indices\": %i\n", firstIndicesAccessor + i);
			fprintf(fp, "                }\n");
		}
		fprintf(fp, "            ]\n");
		fprintf(fp, "        }\n");
		fprintf(fp, "    ],\n");
		fprintf(fp, "    \"accessors\": [\n");
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
			gltf::write(fp, accessor);
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
