#ifndef LIBGLTF_ATTRIBUTES_H
#define LIBGLTF_ATTRIBUTES_H

#include <optional>

#include <nel/misc/types_nl.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct Attributes
{
	std::optional<uint32> position;
	std::optional<uint32> normal;
	std::optional<uint32> texcoord0;

	void write(JsonWriter &writer) const
	{
		if (position)
		{
			writer.writeProperty("POSITION", position.value());
		}
		if (normal)
		{
			writer.writeProperty("NORMAL", normal.value());
		}
		if (texcoord0)
		{
			writer.writeProperty("TEXCOORD_0", texcoord0.value());
		}
	}
};

}

#endif // LIBGLTF_ATTRIBUTES_H
