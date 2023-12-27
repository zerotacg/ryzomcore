#include <libgltf/Asset.h>

#include <libgltf/gltf.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

void Asset::write(JsonWriter &writer) const
{
	writer.writeProperty("asset", info);
	writer.writeProperty("meshes", meshes);
	if (!materials.empty())
	{
		writer.writeProperty("materials", materials);
	}
	if (!textures.empty())
	{
		writer.writeProperty("textures", textures);
	}
	if (!images.empty())
	{
		writer.writeProperty("images", images);
	}
	writer.writeProperty("accessors", accessors);
	writer.writeProperty("bufferViews", bufferViews);
	writer.writeProperty("buffers", buffers);
}

}
