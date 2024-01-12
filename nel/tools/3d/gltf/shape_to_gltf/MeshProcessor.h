#ifndef MESH_PROCESSOR_H
#define MESH_PROCESSOR_H

#include <optional>
#include <vector>
#include <nel/3d/mesh.h>
#include <nel/misc/vector.h>

class MeshProcessor
{
public:
	static std::optional<MeshProcessor> from(NL3D::IShape *shape);

	virtual void process(std::vector<NLMISC::CVector> &vertices, std::vector<NLMISC::CVector> &normals, std::vector<NLMISC::CUV> &textureCoordinates, std::vector<MeshPart> &parts) = 0;
};

#endif // MESH_PROCESSOR_H
