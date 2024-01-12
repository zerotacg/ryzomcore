#include <iostream>
#include <vector>
#include <nel/misc/types_nl.h>
#include <nel/misc/common.h>
#include <nel/misc/cmd_args.h>
#include <nel/misc/bitmap.h>
#include <nel/misc/file.h>
#include <nel/3d/mesh.h>
#include <nel/3d/mesh_mrm.h>
#include <nel/3d/mesh_mrm_skinned.h>
#include <nel/3d/register_3d.h>
#include <nel/3d/scene.h>
#include <nel/3d/texture_file.h>
#include <nel/3d/texture_multi_file.h>
#include <nel/3d/texture_cube.h>
#include <nel/misc/app_context.h>

#include <libgltf/gltf.h>

#include "./shape_to_gltf.h"
#include "./MeshProcessor.h"

using namespace NL3D;
using namespace NLMISC;
using namespace std;

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

		auto meshProcessor = MeshProcessor::from(shape);
		if (meshProcessor)
		{
			meshProcessor->process(vertices, normals, textureCoordinates, parts);
		}
		if (!meshProcessor && !processMesh(shape, vertices, normals, textureCoordinates, parts) && !processMeshMRMSkinned(shape, vertices, normals, textureCoordinates, parts))
		{
			nlwarning("File not a CMesh or CMeshMRMSkinned or CWaterShape");
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
		gltf::JsonWriter gltfWriter = { .file = fp };
		std::vector<gltf::Accessor> accessors = {
			{ .bufferView = 0, .byteOffset = 0, .componentType = gltf::ComponentType::FLOAT, .count = vertices.size(), .type = gltf::AccessorType::VEC3 },
			{ .bufferView = 1, .byteOffset = 0, .componentType = gltf::ComponentType::FLOAT, .count = normals.size(), .type = gltf::AccessorType::VEC3 },
			{ .bufferView = 2, .byteOffset = 0, .componentType = gltf::ComponentType::FLOAT, .count = textureCoordinates.size(), .type = gltf::AccessorType::VEC2 }
		};
		std::vector<gltf::Primitive> primitives;
		std::vector<gltf::Material> materials;
		std::vector<gltf::Texture> textures;
		std::vector<gltf::Image> images;
		for (auto &part : parts)
		{
			gltf::Primitive primitive = {
				.attributes = {
				    .position = 0,
				    .normal = 1,
				    .texcoord0 = 2 },
				.indices = accessors.size(),
			};
			if (!part.textures.empty())
			{
				primitive.material = materials.size();
				for (auto textureFile : part.textures)
				{
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
					gltf::TextureInfo baseColorTexture = { textures.size() };
					for (auto j = 0; j < images.size(); ++j)
					{
						if (images[j].uri == imageUri)
						{
							texture.source = j;
							baseColorTexture.index = j;
						}
					}
					if (texture.source == images.size())
					{
						images.push_back({ .uri = imageUri });
						textures.push_back(texture);
					}
					materials.push_back({ .pbrMetallicRoughness = gltf::MetallicRoughness { baseColorTexture } });
				}
			}
			primitives.push_back(primitive);

			auto &indices = part.indices;
			accessors.push_back({ .bufferView = 3, .byteOffset = outputIndices.getPos(), .componentType = gltf::ComponentType::UNSIGNED_INT, .count = indices.size(), .type = gltf::AccessorType::SCALAR });
			for (auto &element : indices)
			{
				outputIndices.serial(element);
			}
		}
		gltf::Asset asset = {
			.meshes = { { .primitives = primitives } },
			.materials = materials,
			.textures = textures,
			.images = images,
			.nodes = { { .mesh = 0, .translation = { 0.0f, 0.0f, 0.0f } } },
			.scenes = { { .nodes = { 0 } } },
			.accessors = accessors
		};
		asset.bufferViews.push_back({ .buffer = asset.buffers.size(), .byteLength = outputPosition.getPos() });
		asset.buffers.push_back({ .uri = positionFileName, .byteLength = outputPosition.getPos() });

		asset.bufferViews.push_back({ .buffer = asset.buffers.size(), .byteLength = outputNormals.getPos() });
		asset.buffers.push_back({ .uri = normalsFileName, .byteLength = outputNormals.getPos() });

		asset.bufferViews.push_back({ .buffer = asset.buffers.size(), .byteLength = outputTextureCoordinates.getPos() });
		asset.buffers.push_back({ .uri = textureCoordinatesFileName, .byteLength = outputTextureCoordinates.getPos() });

		asset.bufferViews.push_back({ .buffer = asset.buffers.size(), .byteLength = outputIndices.getPos() });
		asset.buffers.push_back({ .uri = indicesFileName, .byteLength = outputIndices.getPos() });

		gltfWriter.write(asset);
		fclose(fp);
		outputPosition.close();
		outputNormals.close();
		outputTextureCoordinates.close();
		outputIndices.close();

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

void fillFileNames(std::vector<string> &textures, ITexture *generic)
{
	if (const auto specific = dynamic_cast<CTextureFile *>(generic))
	{
		const auto &fileName = specific->getFileName();
		nlinfo("CTextureFile %s", fileName.c_str());
		textures.push_back(fileName);
	}
	else if (const auto specific = dynamic_cast<CTextureMultiFile *>(generic))
	{
		nlinfo("CTextureMultiFile count %i", specific->getNumFileName());
		for (auto i = 0; i < specific->getNumFileName(); ++i)
		{
			const auto &fileName = specific->getFileName(i);
			nlinfo("CTextureMultiFile %i %s ", i, fileName.c_str());
			textures.push_back(fileName);
		}
	}
	else if (const auto specific = dynamic_cast<CTextureCube *>(generic))
	{
		nlinfo("CTextureCube");
		fillFileNames(textures, specific->getTexture(CTextureCube::positive_x));
		fillFileNames(textures, specific->getTexture(CTextureCube::negative_x));
		fillFileNames(textures, specific->getTexture(CTextureCube::positive_y));
		fillFileNames(textures, specific->getTexture(CTextureCube::negative_y));
		fillFileNames(textures, specific->getTexture(CTextureCube::positive_z));
		fillFileNames(textures, specific->getTexture(CTextureCube::negative_z));
	}
	else
	{
		nlwarning("Texture type not supported", generic->getClassName().c_str());
	}
}

void fillTextureFileNames(std::vector<string> &textures, const CMaterial &material)
{
	if (material.getBlend())
	{
		nlinfo("Material Blend");
	}
	for (auto textureIndex = 0; textureIndex < IDRV_MAT_MAXTEXTURES; ++textureIndex)
	{
		if (material.texturePresent(textureIndex))
		{
			auto texture = material.getTexture(textureIndex);
			nlinfo("Texture at index %i is %s", textureIndex, texture->getClassName().c_str());
			fillFileNames(textures, texture);
		}
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
		logIndexBufferFormat(indexBuffer.getFormat());

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

void logIndexBufferFormat(const CIndexBuffer::TFormat format)
{
	switch (format)
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
}

bool processMeshMRMSkinned(IShape *shape, vector<CVector> &vertices, vector<CVector> &normals, vector<CUV> &textureCoordinates, vector<MeshPart> &parts)
{
	auto *mesh = dynamic_cast<CMeshMRMSkinned *>(shape);

	if (!mesh) return false;

	nlinfo("File is a CMeshMRMSkinned");

	CVertexBuffer vertexBuffer;
	mesh->getVertexBuffer(vertexBuffer);
	CVertexBufferRead vba;
	vertexBuffer.lock(vba);

	const auto lodCount = mesh->getNbLod();
	const auto lodId = lodCount - 1;
	nlinfo("LodCount %i", lodCount);

	for (auto i = 0; i < vertexBuffer.getNumVertices(); ++i)
	{
		vertices.push_back(*vba.getVertexCoordPointer(i));
		normals.push_back(*vba.getNormalCoordPointer(i));
		textureCoordinates.push_back(*vba.getTexCoordPointer(i));
	}

	const auto meshIn = mesh->getMeshGeom();
	std::vector<CMesh::CSkinWeight> skinWeights;
	meshIn.getSkinWeights(skinWeights);
	const std::vector<CMRMWedgeGeom> &geomorphs = meshIn.getGeomorphs(lodId);
	for (auto renderPass = 0; renderPass < mesh->getNbRdrPass(lodId); ++renderPass)
	{
		CIndexBuffer indexBuffer;
		mesh->getRdrPassPrimitiveBlock(lodId, renderPass, indexBuffer);
		auto materialIndex = mesh->getRdrPassMaterial(lodId, renderPass);
		nlinfo("RenderPasss %i Elements %i Material %i", renderPass, indexBuffer.getNumIndexes(), materialIndex);
		auto material = mesh->getMaterial(materialIndex);
		if (material.getBlend())
		{
			nlinfo("Material Blend");
		}
		vector<string> textures;
		fillTextureFileNames(textures, material);

		CIndexBufferRead iba;
		indexBuffer.lock(iba);
		vector<uint32> indices;
		logIndexBufferFormat(iba.getFormat());
		for (auto i = 0; i < indexBuffer.getNumIndexes(); ++i)
		{
			uint idx = getIndexAt(iba, i);
			// Get the real Vertex (ie not the geomporhed one).
			if (idx < geomorphs.size())
			{
				// Special for Geomorphs: must take The End target vertex.
				idx = geomorphs[idx].End;
			}
			indices.push_back(idx);
		}

		nldebug("index min %i max %i", *min_element(indices.begin(), indices.end()), *max_element(indices.begin(), indices.end()));
		parts.push_back({ .indices = indices, .textures = textures });
	}

	return true;
}
