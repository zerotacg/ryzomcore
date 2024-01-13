#ifndef LIBGLTF_CHANNEL_TARGET_H
#define LIBGLTF_CHANNEL_TARGET_H

#include <string>

#include <libgltf/JsonWriter.h>

namespace gltf {

struct ChannelTarget
{
	std::optional<size_t> node;
	std::string path;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("node", node);
		writer.writeProperty("path", path);
	}
};

}

#endif // LIBGLTF_CHANNEL_TARGET_H
