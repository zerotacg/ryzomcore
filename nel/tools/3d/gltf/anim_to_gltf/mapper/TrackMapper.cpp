#include <mapper/TrackMapper.h>

#include <nel/3d/track.h>
#include <nel/3d/track_sampled_vector.h>
#include <nel/3d/track_sampled_quat.h>

#include <mapper/TrackDefaultQuatMapper.h>
#include <mapper/TrackSampledVectorMapper.h>
#include <mapper/TrackSampledQuatMapper.h>

using namespace NL3D;
using namespace std;

unique_ptr<TrackMapper> TrackMapper::from(ITrack *track)
{
	nldebug("Track is a '%s'", track->getClassName().c_str());

	if (dynamic_cast<CTrackDefaultQuat *>(track))
	{
		return std::make_unique<TrackDefaultQuatMapper>(dynamic_cast<CTrackDefaultQuat *>(track));
	}
	else if (dynamic_cast<CTrackSampledVector *>(track))
	{
		return std::make_unique<TrackSampledVectorMapper>(dynamic_cast<CTrackSampledVector *>(track));
	}
	else if (dynamic_cast<CTrackSampledQuat *>(track))
	{
		return std::make_unique<TrackSampledQuatMapper>(dynamic_cast<CTrackSampledQuat *>(track));
	}

	return std::unique_ptr<TrackMapper> {};
}

void TrackMapper::map(ITrack *source, SampleData &target)
{
	auto mapper = from(source);
	if (mapper)
	{
		mapper->map(target);
	}
}
