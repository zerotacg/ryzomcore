#ifndef MESH_PROCESSOR_H
#define MESH_PROCESSOR_H

#include <memory>
#include <vector>

#include <nel/3d/mesh.h>
#include <nel/misc/vector.h>

#include "MeshPart.h"

class MeshProcessor
{
public:
	virtual ~MeshProcessor() = default;

	virtual void process(std::vector<NLMISC::CVector> &vertices, std::vector<NLMISC::CVector> &normals, std::vector<NLMISC::CUV> &uvs, std::vector<MeshPart> &parts) = 0;

	static std::unique_ptr<MeshProcessor> from(NL3D::IShape *shape);
};

#endif // MESH_PROCESSOR_H
