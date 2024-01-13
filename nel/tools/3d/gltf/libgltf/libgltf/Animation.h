#ifndef LIBGLTF_ANIMATION_H
#define LIBGLTF_ANIMATION_H

#include <string>
#include <vector>

#include <libgltf/Channel.h>
#include <libgltf/Sampler.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct Animation
{
	std::string name;
	std::vector<Channel> channels;
	std::vector<Sampler> samplers;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("name", name);
		writer.writeProperty("channels", samplers);
	}
};

}

#endif // LIBGLTF_ANIMATION_H
