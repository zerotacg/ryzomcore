#ifndef TRACK_MAPPER_H
#define TRACK_MAPPER_H

#include <memory>

#include "nel/3d/track.h"

class TrackMapper
{
public:
	virtual ~TrackMapper() = default;

//	virtual void process(Mesh& output) = 0;

	static std::unique_ptr<TrackMapper> from(NL3D::ITrack *track);
};

#endif // TRACK_MAPPER_H
