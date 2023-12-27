#ifndef LIBGLTF_MATERIAL_H
#define LIBGLTF_MATERIAL_H

#include <map>
#include <string>

#include <libgltf/other.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct Material
{
	std::string name;
	MetallicRoughness pbrMetallicRoughness;
	bool hasPbrMetallicRoughness;
	// NormalTextureInfo normalTexture;
	// OcclusionTextureInfo occlusionTexture;
	// TextureInfo emissiveTexture;
	// float emissiveFactor[3];
	AlphaMode alphaMode;
	// float alphaCutoff;
	// bool doubleSided;
	std::map<std::string,std::string> extras;

	[[nodiscard]] bool hasDefaultAlphaMode() const { return alphaMode == AlphaMode::OPAQUE; }

	void write(JsonWriter &writer) const
	{
		if(!name.empty())
		{
			writer.writeProperty("name", name);
		}
		if (!hasDefaultAlphaMode())
		{
			writer.writeProperty("alphaMode", AlphaModeNames[static_cast<int>(alphaMode)]);
		}
		if(hasPbrMetallicRoughness)
		{
			writer.writeProperty("pbrMetallicRoughness", pbrMetallicRoughness);
		}
	}
};

}

#endif // LIBGLTF_MATERIAL_H
