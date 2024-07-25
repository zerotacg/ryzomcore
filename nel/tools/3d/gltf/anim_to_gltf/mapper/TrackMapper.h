#ifndef TRACK_MAPPER_H
#define TRACK_MAPPER_H

#include <memory>

#include <nel/3d/track.h>

#include <SampleData.h>

class TrackMapper
{
public:
	virtual ~TrackMapper() = default;

	virtual void map(SampleData& target) = 0;

	static std::unique_ptr<TrackMapper> from(NL3D::ITrack *track);
};

#endif // TRACK_MAPPER_H
