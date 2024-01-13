#ifndef LIBGLTF_NODE_H
#define LIBGLTF_NODE_H

#include <nel/misc/types_nl.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct Node
{
	std::string name;
	std::optional<size_t> mesh;
	std::optional<size_t> skin;
	std::vector<float> translation;
	std::vector<float> rotation;
	std::vector<size_t> children;


	void write(JsonWriter &writer) const
	{
		if(!name.empty())
		{
			writer.writeProperty("name", name);
		}
		writer.writeProperty("mesh", mesh);
		writer.writeProperty("skin", skin);
		if(!translation.empty())
		{
			writer.writeProperty("translation", translation);
		}
		if(!rotation.empty())
		{
			writer.writeProperty("rotation", rotation);
		}
		if(!children.empty())
		{
			writer.writeProperty("children", children);
		}
	}
};

}

#endif // LIBGLTF_NODE_H
