#ifndef MESH_MRM_SKINNED_PROCESSOR_H
#define MESH_MRM_SKINNED_PROCESSOR_H

#include <nel/3d/mesh_mrm_skinned.h>
#include <nel/3d/skeleton_shape.h>

#include "MeshProcessor.h"

class MeshMRMSkinnedProcessor : public MeshProcessor
{
public:
	explicit MeshMRMSkinnedProcessor(NL3D::CMeshMRMSkinned *source, NL3D::CSkeletonShape *optionalSkeleton)
	    : mesh(source), skeleton(optionalSkeleton)
	{
	}

	void process(Mesh& output) override;

private:
	NL3D::CMeshMRMSkinned *mesh;
	NL3D::CSkeletonShape *skeleton;
};

#endif // MESH_MRM_SKINNED_PROCESSOR_H
