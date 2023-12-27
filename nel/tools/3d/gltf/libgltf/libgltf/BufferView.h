#ifndef LIBGLTF_BUFFER_VIEW_H
#define LIBGLTF_BUFFER_VIEW_H

#include <nel/misc/types_nl.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct BufferView
{
	size_t buffer;
	sint32 byteLength;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("buffer", buffer);
		writer.writeProperty("byteLength", byteLength);
	}
};

}

#endif // LIBGLTF_BUFFER_VIEW_H
