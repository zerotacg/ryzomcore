#ifndef LIBGLTF_CHANNEL_TARGET_PATH_H
#define LIBGLTF_CHANNEL_TARGET_PATH_H

#include <string>

namespace gltf {

enum class ChannelTargetPath
{
	TRANSLATION = 0,
	ROTATION,
	SCALE,
	WEIGHTS
};
const std::string ChannelTargetPathNames[] = { "translation", "rotation", "scale", "weights" };

}

#endif // LIBGLTF_CHANNEL_TARGET_PATH_H
