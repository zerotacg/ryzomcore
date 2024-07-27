#include <mapper/TrackSampledQuatMapper.h>

using namespace NL3D;
using namespace NLMISC;
using namespace std;

void TrackSampledQuatMapper::map(ChannelData &output)
{
	output.type = gltf::AccessorType::VEC4;
	output.interpolation = gltf::Interpolation::LINEAR;
	track->fill(output.quaternion);

	auto count = output.quaternion.size();
	auto begin = track->getBeginTime();
	auto end = track->getEndTime();
	auto delta = (end - begin) / count;
	for( auto i = 0; i < count; ++i) {
		auto t = begin + i * delta;
		output.time.push_back(t);
	}
}
