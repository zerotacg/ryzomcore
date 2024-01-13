#include "MeshProcessor.h"

#include <nel/3d/mesh_mrm_skinned.h>
#include <nel/3d/water_shape.h>

#include "MeshMRMSkinnedProcessor.h"
#include "WaterShapeProcessor.h"
#include "nel/3d/skeleton_shape.h"

using namespace NL3D;
using namespace std;

unique_ptr<MeshProcessor> MeshProcessor::from(IShape *shape, IShape *skeleton)
{
	if (dynamic_cast<CMeshMRMSkinned *>(shape))
	{
		return std::make_unique<MeshMRMSkinnedProcessor>(dynamic_cast<CMeshMRMSkinned *>(shape), dynamic_cast<CSkeletonShape *>(skeleton));
	}
	if (dynamic_cast<CWaterShape *>(shape))
	{
		return std::make_unique<WaterShapeProcessor>(dynamic_cast<CWaterShape *>(shape));
	}
	return std::unique_ptr<MeshProcessor>{};
}