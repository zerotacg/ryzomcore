#ifndef MESH_MRM_PROCESSOR_H
#define MESH_MRM_PROCESSOR_H

#include <nel/3d/mesh_mrm.h>

#include "MeshProcessor.h"

class MeshMRMProcessor : public MeshProcessor
{
public:
	explicit MeshMRMProcessor(NL3D::CMeshMRM *source)
	    : mesh(source)
	{
	}

	void process(Mesh& output) override;

private:
	NL3D::CMeshMRM *mesh;
};

#endif // MESH_MRM_PROCESSOR_H
