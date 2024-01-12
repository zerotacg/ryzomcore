#include "WaterShapeProcessor.h"

using namespace NL3D;
using namespace NLMISC;
using namespace std;

void WaterShapeProcessor::process(vector<CVector> &vertices, vector<CVector> &normals, vector<CUV> &textureCoordinates, vector<MeshPart> &parts)
{
	nlinfo("File is a CWaterShape");

	auto & polygon = mesh->getShape();
	for(auto & element: polygon.Vertices)
	{
		vertices.push_back(element.asVector());
	}
}
