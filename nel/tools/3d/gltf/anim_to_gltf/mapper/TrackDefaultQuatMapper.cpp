#include <mapper/TrackDefaultQuatMapper.h>

using namespace NL3D;
using namespace NLMISC;
using namespace std;

void TrackDefaultQuatMapper::map(SampleData &output)
{
	output.type = gltf::AccessorType::VEC4;
	output.interpolation = gltf::Interpolation::LINEAR;
	output.time.push_back(track->getBeginTime());
	output.quaternion.push_back(track->getDefaultValue());
}
