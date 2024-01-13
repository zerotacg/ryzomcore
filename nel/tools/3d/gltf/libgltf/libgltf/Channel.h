#ifndef LIBGLTF_CHANNEL_H
#define LIBGLTF_CHANNEL_H

#include <string>

#include <libgltf/ChannelTarget.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct Channel
{
	size_t sampler;
	ChannelTarget target;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("sampler", sampler);
		writer.writeProperty("target", target);
	}
};

}

#endif // LIBGLTF_CHANNEL_H
