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
		writer.writeProperty("POSITION", position);
		writer.writeProperty("NORMAL", normal);
		writer.writeProperty("TEXCOORD_0", texcoord0);
	}
};

}

#endif // LIBGLTF_ATTRIBUTES_H
