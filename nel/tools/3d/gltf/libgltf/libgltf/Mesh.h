#ifndef LIBGLTF_MESH_H
#define LIBGLTF_MESH_H

#include <vector>

#include <libgltf/Primitive.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct Mesh
{
	std::vector<Primitive> primitives;
	void write(JsonWriter &writer) const
	{
		writer.writeProperty("primitives", primitives);
	}
};

}

#endif // LIBGLTF_MESH_H
