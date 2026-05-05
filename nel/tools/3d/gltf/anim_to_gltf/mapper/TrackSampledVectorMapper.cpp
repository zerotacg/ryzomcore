#include <mapper/TrackSampledVectorMapper.h>

using namespace NL3D;
using namespace NLMISC;
using namespace std;

void TrackSampledVectorMapper::map(ChannelData &output)
{
	output.type = gltf::AccessorType::VEC3;
	output.interpolation = gltf::Interpolation::LINEAR;
	track->fill(output.vector);

	auto count = output.vector.size();
	auto begin = track->getBeginTime();
	auto end = track->getEndTime();
	auto delta = (end - begin) / count;
	for( auto i = 0; i < count; ++i) {
		auto t = begin + i * delta;
		output.time.push_back(t);
	}
}
