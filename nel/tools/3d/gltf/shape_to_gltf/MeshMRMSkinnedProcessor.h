#ifndef MESH_MRM_SKINNED_PROCESSOR_H
#define MESH_MRM_SKINNED_PROCESSOR_H

#include <nel/3d/mesh_mrm_skinned.h>

#include "MeshProcessor.h"


struct MeshMRMSkinnedProcessor : MeshProcessor {
	explicit MeshMRMSkinnedProcessor(NL3D::CMeshMRMSkinned *source)
		: mesh(source)
	{
	}

	void process(std::vector<NLMISC::CVector> &vertices, std::vector<NLMISC::CVector> &normals, std::vector<NLMISC::CUV> &textureCoordinates, std::vector<MeshPart> &parts) override;

private:
	NL3D::CMeshMRMSkinned *mesh;

};



#endif //MESH_MRM_SKINNED_PROCESSOR_H
