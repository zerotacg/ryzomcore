#include "MeshMRMSkinnedProcessor.h"

#include "shape_to_gltf.h"

using namespace NL3D;
using namespace NLMISC;
using namespace std;

void MeshMRMSkinnedProcessor::process(vector<CVector> &vertices, vector<CVector> &normals, vector<CUV> &textureCoordinates, vector<MeshPart> &parts)
{
	nlinfo("File is a CWaterShape");

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
}
