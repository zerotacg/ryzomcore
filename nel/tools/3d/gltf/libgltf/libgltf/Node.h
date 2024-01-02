#ifndef LIBGLTF_NODE_H
#define LIBGLTF_NODE_H

#include <nel/misc/types_nl.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct Node
{
	std::string name;
	std::optional<size_t> mesh;
	std::vector<float> translation;


	void write(JsonWriter &writer) const
	{
		if(!name.empty())
		{
			writer.writeProperty("name", name);
		}
		writer.writeProperty("mesh", mesh);
		if(!translation.empty())
		{
			writer.writeProperty("translation", translation);
		}
	}
};

}

#endif // LIBGLTF_NODE_H
