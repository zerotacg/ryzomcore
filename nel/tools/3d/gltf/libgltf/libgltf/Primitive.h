#ifndef LIBGLTF_PRIMITIVE_H
#define LIBGLTF_PRIMITIVE_H

#include <libgltf/Attributes.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct Primitive
{
	Attributes attributes;
	size_t indices;
	size_t material;
	bool hasIndices;
	bool hasMaterial;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("attributes", attributes);
		if (hasIndices)
		{
			writer.writeProperty("indices", indices);
		}
		if (hasMaterial)
		{
			writer.writeProperty("material", material);
		}
	}
};

}

#endif // LIBGLTF_PRIMITIVE_H
