#ifndef MESH_PROCESSOR_H
#define MESH_PROCESSOR_H

#include <memory>

#include <nel/3d/mesh.h>

#include "Mesh.h"

class MeshProcessor
{
public:
	virtual ~MeshProcessor() = default;

	virtual void process(Mesh& output) = 0;

	static std::unique_ptr<MeshProcessor> from(NL3D::IShape *shape);
};

#endif // MESH_PROCESSOR_H
