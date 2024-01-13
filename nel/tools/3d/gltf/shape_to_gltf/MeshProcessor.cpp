#include "MeshProcessor.h"

#include <nel/3d/mesh_mrm.h>
#include <nel/3d/mesh_mrm_skinned.h>
#include <nel/3d/mesh_multi_lod.h>
#include <nel/3d/skeleton_shape.h>
#include <nel/3d/water_shape.h>

#include "MeshMRMProcessor.h"
#include "MeshMRMSkinnedProcessor.h"
#include "MeshMultiLodProcessor.h"
#include "WaterShapeProcessor.h"

using namespace NL3D;
using namespace std;

unique_ptr<MeshProcessor> MeshProcessor::from(IShape *shape, IShape *skeleton)
{
	if (dynamic_cast<CMeshMRM *>(shape))
	{
		return std::make_unique<MeshMRMProcessor>(dynamic_cast<CMeshMRM *>(shape));
	}
	if (dynamic_cast<CMeshMRM *>(shape))
	{
		return std::make_unique<MeshMRMSkinnedProcessor>(dynamic_cast<CMeshMRMSkinned *>(shape), dynamic_cast<CSkeletonShape *>(skeleton));
	}
	if (dynamic_cast<CMeshMultiLod *>(shape))
	{
		return std::make_unique<MeshMultiLodProcessor>(dynamic_cast<CMeshMultiLod *>(shape));
	}
	if (dynamic_cast<CWaterShape *>(shape))
	{
		return std::make_unique<WaterShapeProcessor>(dynamic_cast<CWaterShape *>(shape));
	}
	return std::unique_ptr<MeshProcessor>{};
}