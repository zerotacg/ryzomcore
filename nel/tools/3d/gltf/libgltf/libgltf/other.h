#ifndef LIBGLTF_OTHER_H
#define LIBGLTF_OTHER_H

#include <nel/misc/types_nl.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

enum class ComponentType : uint32
{
	SIGNED_BYTE = 5120,
	UNSIGNED_BYTE = 5121,
	SIGNED_SHORT = 5122,
	UNSIGNED_SHORT = 5123,
	UNSIGNED_INT = 5125,
	FLOAT = 5126
};

enum class AccessorType
{
	SCALAR = 0,
	VEC2,
	VEC3,
	VEC4,
	MAT2,
	MAT3,
	MAT4
};
const std::string AccessorTypeNames[] = { "SCALAR", "VEC2", "VEC3", "VEC4", "MAT2", "MAT3", "MAT4" };

struct TextureInfo
{
	size_t index;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("index", index);
	}
};

struct NormalTextureInfo
{
	uint32 index;
	float scale;
};

struct OcclusionTextureInfo
{
	uint32 index;
	float strength;
};

struct MetallicRoughness
{
	// float baseColorFactor[4];
	TextureInfo baseColorTexture;
	// float metallicFactor;
	// float roughnessFactor;
	// TextureInfo metallicRoughnessTexture;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("baseColorTexture", baseColorTexture);
	}
};

enum class AlphaMode
{
	OPAQUE,
	MASK,
	BLEND
};
const std::string AlphaModeNames[] = { "OPAQUE", "MASK", "BLEND" };

}

#endif // LIBGLTF_OTHER_H
