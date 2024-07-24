#include "mapper/TrackMapper.h"

#include "nel/3d/track_sampled_vector.h"

using namespace NL3D;
using namespace std;

unique_ptr<TrackMapper> TrackMapper::from(ITrack *track)
{
	if (dynamic_cast<CTrackSampledVector *>(track))
	{
//		return std::make_unique<MeshMRMProcessor>(dynamic_cast<CMeshMRM *>(track));
		return std::unique_ptr<TrackMapper>{};
	}

	return std::unique_ptr<TrackMapper>{};
}