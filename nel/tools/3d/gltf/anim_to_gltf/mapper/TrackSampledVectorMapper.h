#ifndef TRACK_SAMPLED_VECTOR_MAPPER_H
#define TRACK_SAMPLED_VECTOR_MAPPER_H

#include <nel/3d/track_sampled_vector.h>

#include <mapper/TrackMapper.h>

class TrackSampledVectorMapper : public TrackMapper
{
public:
	explicit TrackSampledVectorMapper(NL3D::CTrackSampledVector *source)
	    : track(source)
	{
	}

	void map(ChannelData & target) override;

private:
	NL3D::CTrackSampledVector *track;
};

#endif // TRACK_SAMPLED_VECTOR_MAPPER_H
