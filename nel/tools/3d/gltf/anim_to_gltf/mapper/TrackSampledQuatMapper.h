#ifndef TRACK_SAMPLED_QUAT_MAPPER_H
#define TRACK_SAMPLED_QUAT_MAPPER_H

#include <nel/3d/track_sampled_quat.h>

#include <mapper/TrackMapper.h>

class TrackSampledQuatMapper : public TrackMapper
{
public:
	explicit TrackSampledQuatMapper(NL3D::CTrackSampledQuat *source)
	    : track(source)
	{
	}

	void map(SampleData& target) override;

private:
	NL3D::CTrackSampledQuat *track;
};

#endif // TRACK_SAMPLED_QUAT_MAPPER_H
