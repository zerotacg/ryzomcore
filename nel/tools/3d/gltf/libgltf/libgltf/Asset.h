#ifndef LIBGLTF_ASSET_H
#define LIBGLTF_ASSET_H

#include <vector>

#include <libgltf/Accessor.h>
#include <libgltf/Buffer.h>
#include <libgltf/BufferView.h>
#include <libgltf/Image.h>
#include <libgltf/Material.h>
#include <libgltf/Mesh.h>
#include <libgltf/Texture.h>

namespace gltf {

struct Asset
{
	std::vector<Mesh> meshes;
	std::vector<Material> materials;
	std::vector<Texture> textures;
	std::vector<Image> images;
	std::vector<Accessor> accessors;
	std::vector<BufferView> bufferViews;
	std::vector<Buffer> buffers;

	void write(JsonWriter &writer) const;
};

}

#endif // LIBGLTF_ASSET_H
