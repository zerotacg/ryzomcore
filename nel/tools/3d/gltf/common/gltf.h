#ifndef GLTF_H
#define GLTF_H

#include "gltf.h"

#include <iostream>
#include <vector>

#include <nel/misc/types_nl.h>

namespace gltf {

struct JsonWriter
{
	FILE *file;
	std::vector<std::string> propertyPrefix;

	template <class T, class Allocator>
	void write(const std::vector<T, Allocator> &container)
	{
		fprintf(file, "[");
		for (auto it = container.begin(); it != container.end(); ++it)
		{
			if (it != container.begin())
			{
				fprintf(file, ", ");
			}
			write(*it);
		}
		fprintf(file, "]");
	}

	template <class T>
	void write(const T &object)
	{
		fprintf(file, "{ ");
		propertyPrefix.push_back("");
		object.write(*this);
		propertyPrefix.pop_back();
		fprintf(file, " }");
	}

	void write(const std::string &value)
	{
		fprintf(file, R"("%s")", value.c_str());
	}

	void write(const uint32 &value)
	{
		fprintf(file, "%i", value);
	}
	void write(const sint32 &value)
	{
		fprintf(file, "%i", value);
	}

	void write(const size_t &value)
	{
		fprintf(file, "%lu", value);
	}

	template <class T>
	void writeProperty(const std::string &key, const T &value)
	{
		fprintf(file, "%s", propertyPrefix.back().c_str());
		write(key);
		fprintf(file, ": ");
		write(value);
		propertyPrefix.back() = ", ";
	}
};

struct Info
{
	std::string version = "2.0";

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("version", version);
	}
} info;

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

struct Accessor
{
	uint32 bufferView;
	sint32 byteOffset;
	ComponentType componentType;
	size_t count;
	AccessorType type;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("bufferView", bufferView);
		if ( byteOffset > 0)
		{
			writer.writeProperty("byteOffset", byteOffset);
		}
		writer.writeProperty("componentType", static_cast<uint32>(componentType));
		writer.writeProperty("count", count);
		writer.writeProperty("type", AccessorTypeNames[static_cast<int>(type)]);
	}
};

struct Image
{
	std::string uri;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("uri", uri);
	}
};

struct TextureInfo
{
	size_t index;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("index", index);
	}
};

struct Texture
{
	size_t source;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("source", source);
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

struct Material
{
	MetallicRoughness pbrMetallicRoughness;
	// NormalTextureInfo normalTexture;
	// OcclusionTextureInfo occlusionTexture;
	// TextureInfo emissiveTexture;
	// float emissiveFactor[3];
	AlphaMode alphaMode;
	// float alphaCutoff;
	// bool doubleSided;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("alphaMode", AlphaModeNames[static_cast<int>(alphaMode)]);
		writer.writeProperty("pbrMetallicRoughness", pbrMetallicRoughness);
	}
};

struct Attributes
{
	uint32 position;
	uint32 normal;
	uint32 texcoord0;
	bool hasPosition;
	bool hasNormal;
	bool hasTexcoord0;

	void write(JsonWriter &writer) const
	{
		if (hasPosition)
		{
			writer.writeProperty("POSITION", position);
		}
		if (hasNormal)
		{
			writer.writeProperty("NORMAL", normal);
		}
		if (hasTexcoord0)
		{
			writer.writeProperty("TEXCOORD_0", texcoord0);
		}
	}
};

struct Primitive
{
	Attributes attributes;
	size_t indices;
	size_t material;
	bool hasIndices;
	bool hasMaterial;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("attributes", attributes);
		if (hasIndices)
		{
			writer.writeProperty("indices", indices);
		}
		if (hasMaterial)
		{
			writer.writeProperty("material", material);
		}
	}
};

struct Mesh
{
	std::vector<Primitive> primitives;
	void write(JsonWriter &writer) const
	{
		writer.writeProperty("primitives", primitives);
	}
};

struct BufferView
{
	size_t buffer;
	sint32 byteLength;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("buffer", buffer);
		writer.writeProperty("byteLength", byteLength);
	}
};

struct Buffer
{
	std::string uri;
	sint32 byteLength;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("uri", uri);
		writer.writeProperty("byteLength", byteLength);
	}
};

struct Asset
{
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
	std::vector<Image> images;
	std::vector<Accessor> accessors;
	std::vector<BufferView> bufferViews;
	std::vector<Buffer> buffers;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("asset", info);
		writer.writeProperty("meshes", meshes);
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
};

}

#endif // GLTF_H
