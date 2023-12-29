#ifndef LIBGLTF_SCENE_H
#define LIBGLTF_SCENE_H

#include <nel/misc/types_nl.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct Scene
{
	std::string name;
	std::vector<size_t> nodes;

	void write(JsonWriter &writer) const
	{
		if(!name.empty())
		{
			writer.writeProperty("name", name);

		}
		if(!nodes.empty())
		{
			writer.writeProperty("nodes", nodes);
		}
	}
};

}

#endif // LIBGLTF_SCENE_H
