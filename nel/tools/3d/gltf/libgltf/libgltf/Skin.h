#ifndef LIBGLTF_SKIN_H
#define LIBGLTF_SKIN_H

#include <optional>

#include <nel/misc/types_nl.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct Skin
{
	std::optional<size_t> skeleton;
	std::vector<size_t> joints;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("skeleton", skeleton);
		if(!joints.empty())
		{
			writer.writeProperty("joints", joints);
		}
	}
};

}

#endif // LIBGLTF_SKIN_H
