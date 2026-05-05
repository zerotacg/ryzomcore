#ifndef LIBGLTF_PRIMITIVE_H
#define LIBGLTF_PRIMITIVE_H

#include <optional>
#include <libgltf/Attributes.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct Primitive
{
	Attributes attributes;
	std::optional<size_t> indices;
	std::optional<size_t> material;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("attributes", attributes);
		writer.writeProperty("indices", indices);
		writer.writeProperty("material", material);
	}
};

}

#endif // LIBGLTF_PRIMITIVE_H
