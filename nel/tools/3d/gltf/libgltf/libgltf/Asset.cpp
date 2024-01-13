#include <libgltf/Asset.h>

#include <libgltf/gltf.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

void Asset::write(JsonWriter &writer) const
{
	writer.writeProperty("asset", info);
	if (!meshes.empty())
	{
		writer.writeProperty("meshes", meshes);
	}
	if (!nodes.empty())
	{
		writer.writeProperty("nodes", nodes);
	}
	if (!skins.empty())
	{
		writer.writeProperty("skins", skins);
	}
	if (!scenes.empty())
	{
		writer.writeProperty("scenes", scenes);
	}
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
	if (!accessors.empty())
	{
		writer.writeProperty("accessors", accessors);
	}
	if (!bufferViews.empty())
	{
		writer.writeProperty("bufferViews", bufferViews);
	}
	if (!buffers.empty())
	{
		writer.writeProperty("buffers", buffers);
	}
}

}
