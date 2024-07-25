#include <mapper/TrackMapper.h>

#include <nel/3d/track_sampled_vector.h>

#include <mapper/TrackSampledVectorMapper.h>

using namespace NL3D;
using namespace std;

unique_ptr<TrackMapper> TrackMapper::from(ITrack *track)
{
	nlinfo("File is a '%s''", track->getClassName().c_str());

	if (dynamic_cast<CTrackSampledVector *>(track))
	{
		return std::make_unique<TrackSampledVectorMapper>(dynamic_cast<CTrackSampledVector *>(track));
	}

	return std::unique_ptr<TrackMapper>{};
}