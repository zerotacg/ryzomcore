#ifndef LIBGLTF_MESH_H
#define LIBGLTF_MESH_H

#include <vector>

#include <libgltf/Primitive.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct Mesh
{
	std::vector<Primitive> primitives;
	std::vector<size_t> weights;
	std::string name;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("primitives", primitives);
		writer.writeProperty("weights", weights);
		if (!name.empty())
		{
			writer.writeProperty("name", name);
		}
	}
};

}

#endif // LIBGLTF_MESH_H
