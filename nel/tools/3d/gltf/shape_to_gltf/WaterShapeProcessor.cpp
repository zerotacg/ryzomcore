#include "WaterShapeProcessor.h"

using namespace NL3D;
using namespace NLMISC;
using namespace std;

void WaterShapeProcessor::process(Mesh& output)
{
	nlinfo("File is a CWaterShape");

	auto & polygon = mesh->getShape();
	for(auto & element: polygon.Vertices)
	{
		output.vertices.push_back(element.asVector());
	}
}
