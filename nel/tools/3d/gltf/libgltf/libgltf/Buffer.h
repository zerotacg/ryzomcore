#ifndef LIBGLTF_BUFFER_H
#define LIBGLTF_BUFFER_H

#include <string>

#include <nel/misc/types_nl.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct Buffer
{
	std::string uri;
	sint32 byteLength;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("uri", uri);
		writer.writeProperty("byteLength", byteLength);
	}
};

}

#endif // LIBGLTF_BUFFER_H
