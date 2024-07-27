#ifndef TRACK_DEFAULT_QUAT_MAPPER_H
#define TRACK_DEFAULT_QUAT_MAPPER_H

#include <nel/3d/track.h>

#include <mapper/TrackMapper.h>

class TrackDefaultQuatMapper : public TrackMapper
{
public:
	explicit TrackDefaultQuatMapper(NL3D::CTrackDefaultQuat *source)
	    : track(source)
	{
	}

	void map(SampleData& target) override;

private:
	NL3D::CTrackDefaultQuat *track;
};

#endif // TRACK_DEFAULT_QUAT_MAPPER_H
