#include "MeshMultiLodProcessor.h"


#include "shape_to_gltf.h"

using namespace NL3D;
using namespace NLMISC;
using namespace std;

void MeshMultiLodProcessor::process(Mesh &output)
{
	nlinfo("File is a CMeshMultiLod");

	CVertexBuffer vertexBuffer;
	const uint slot = 0;
	const auto &meshIn = mesh->getMeshGeom(slot);
}
