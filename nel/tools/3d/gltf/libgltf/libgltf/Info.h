#ifndef LIBGLTF_INFO_H
#define LIBGLTF_INFO_H

#include <string>

#include <libgltf/JsonWriter.h>

namespace gltf {

struct Info
{
	std::string version;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("version", version);
	}
};

}

#endif // LIBGLTF_INFO_H
