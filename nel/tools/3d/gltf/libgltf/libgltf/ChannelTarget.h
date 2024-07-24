#ifndef LIBGLTF_CHANNEL_TARGET_H
#define LIBGLTF_CHANNEL_TARGET_H

#include <string>

#include <libgltf/ChannelTargetPath.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct ChannelTarget
{
	std::optional<size_t> node;
	ChannelTargetPath path;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("node", node);
		writer.writeProperty("path", ChannelTargetPathNames[static_cast<int>(path)]);
	}
};

}

#endif // LIBGLTF_CHANNEL_TARGET_H
