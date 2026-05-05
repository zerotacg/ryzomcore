#ifndef TRACK_MAPPER_H
#define TRACK_MAPPER_H

#include <memory>

#include <nel/3d/track.h>

#include <ChannelData.h>

class TrackMapper
{
public:
	virtual ~TrackMapper() = default;

	virtual void map(ChannelData & target) = 0;

	static std::unique_ptr<TrackMapper> from(NL3D::ITrack *track);

	static void map(NL3D::ITrack *source, ChannelData & target);
};

#endif // TRACK_MAPPER_H
