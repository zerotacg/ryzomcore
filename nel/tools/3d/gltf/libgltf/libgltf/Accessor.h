#ifndef LIBGLTF_ACCESSOR_H
#define LIBGLTF_ACCESSOR_H

#include <nel/misc/types_nl.h>

#include <libgltf/other.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct Accessor
{
	uint32 bufferView;
	sint32 byteOffset;
	ComponentType componentType;
	size_t count;
	AccessorType type;

	void write(JsonWriter &writer) const;
};

}

#endif // LIBGLTF_ACCESSOR_H
