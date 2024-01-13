#ifndef MESH_MULTI_LOD_PROCESSOR_H
#define MESH_MULTI_LOD_PROCESSOR_H

#include <nel/3d/mesh_multi_lod.h>

#include "MeshProcessor.h"

class MeshMultiLodProcessor : public MeshProcessor
{
public:
	explicit MeshMultiLodProcessor(NL3D::CMeshMultiLod *source)
	    : mesh(source)
	{
	}

	void process(Mesh& output) override;

private:
	NL3D::CMeshMultiLod *mesh;
};

#endif // MESH_MULTI_LOD_PROCESSOR_H
