#ifndef LIBGLTF_IMAGE_H
#define LIBGLTF_IMAGE_H

#include <string>

#include <libgltf/JsonWriter.h>

namespace gltf {

struct Image
{
	std::string uri;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("uri", uri);
	}
};

}

#endif // LIBGLTF_IMAGE_H
