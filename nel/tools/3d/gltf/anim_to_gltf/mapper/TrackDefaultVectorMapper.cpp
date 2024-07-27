#include <mapper/TrackDefaultVectorMapper.h>

using namespace NL3D;
using namespace NLMISC;
using namespace std;

void TrackDefaultVectorMapper::map(SampleData &output)
{
	output.type = gltf::AccessorType::VEC3;
	output.interpolation = gltf::Interpolation::LINEAR;
	output.time.push_back(track->getBeginTime());
	output.vector.push_back(track->getDefaultValue());
}
