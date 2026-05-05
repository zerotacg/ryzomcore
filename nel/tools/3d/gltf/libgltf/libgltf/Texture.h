#ifndef LIBGLTF_TEXTURE_H
#define LIBGLTF_TEXTURE_H

#include <nel/misc/types_nl.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct Texture
{
	size_t source;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("source", source);
	}
};

}

#endif // LIBGLTF_TEXTURE_H
