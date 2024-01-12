#include "MeshProcessor.h"

#include <nel/3d/water_shape.h>

#include "WaterShapeProcessor.h"

std::optional<MeshProcessor> MeshProcessor::from(NL3D::IShape *shape)
{
	if (dynamic_cast<NL3D::CWaterShape *>(shape))
	{
		return WaterShapeProcessor(dynamic_cast<NL3D::CWaterShape *>(shape));
	}
	return std::nullopt;
}