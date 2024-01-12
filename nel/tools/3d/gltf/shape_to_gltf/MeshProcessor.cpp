#include "MeshProcessor.h"

#include <nel/3d/water_shape.h>

#include "WaterShapeProcessor.h"

using namespace std;

unique_ptr<MeshProcessor> MeshProcessor::from(NL3D::IShape *shape)
{
	if (dynamic_cast<NL3D::CWaterShape *>(shape))
	{
		return std::make_unique<WaterShapeProcessor>(dynamic_cast<NL3D::CWaterShape *>(shape));
	}
	return std::unique_ptr<MeshProcessor>{};
}