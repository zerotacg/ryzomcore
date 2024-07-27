#ifndef TRACK_DEFAULT_VECTOR_MAPPER_H
#define TRACK_DEFAULT_VECTOR_MAPPER_H

#include <nel/3d/track.h>

#include <mapper/TrackMapper.h>

class TrackDefaultVectorMapper : public TrackMapper
{
public:
	explicit TrackDefaultVectorMapper(NL3D::CTrackDefaultVector *source)
	    : track(source)
	{
	}

	void map(ChannelData & target) override;

private:
	NL3D::CTrackDefaultVector *track;
};

#endif // TRACK_DEFAULT_VECTOR_MAPPER_H
