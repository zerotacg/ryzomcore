#ifndef MESH_H
#define MESH_H

#include <vector>

#include <nel/misc/vector.h>
#include <nel/misc/uv.h>
#include <nel/misc/quat.h>

#include "MeshPart.h"

struct Mesh
{
	std::vector<NLMISC::CVector> vertices;
	std::vector<NLMISC::CVector> normals;
	std::vector<NLMISC::CUV> uvs;
	std::vector<NLMISC::CQuat> weights;
	std::vector<NLMISC::CQuatT<uint8>> joints;

	std::vector<MeshPart> parts;
};

#endif //MESH_H
