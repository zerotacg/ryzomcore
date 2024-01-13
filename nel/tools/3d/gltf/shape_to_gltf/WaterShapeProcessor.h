#ifndef WATER_SHAPE_PROCESSOR_H
#define WATER_SHAPE_PROCESSOR_H

#include <nel/3d/water_shape.h>

#include "MeshProcessor.h"

class WaterShapeProcessor : public MeshProcessor
{
public:
	explicit WaterShapeProcessor(NL3D::CWaterShape *source)
	    : mesh(source)
	{
	}

	void process(std::vector<NLMISC::CVector> &vertices, std::vector<NLMISC::CVector> &normals, std::vector<NLMISC::CUV> &textureCoordinates, std::vector<MeshPart> &parts) override;

private:
	NL3D::CWaterShape *mesh;
};

#endif // WATER_SHAPE_PROCESSOR_H
