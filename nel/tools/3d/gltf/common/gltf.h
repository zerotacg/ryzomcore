#ifndef GLTF_H
#define GLTF_H

#include <iostream>
#include <vector>

#include <nel/misc/types_nl.h>

namespace gltf {

enum ComponentType : uint32
{
	SIGNED_BYTE = 5120,
	UNSIGNED_BYTE = 5121,
	SIGNED_SHORT = 5122,
	UNSIGNED_SHORT = 5123,
	UNSIGNED_INT = 5125,
	FLOAT = 5126
};
enum AccessorType
{
	SCALAR = 0,
	VEC2,
	VEC3,
	VEC4,
	MAT2,
	MAT3,
	MAT4
};
const char *AccessorTypeNames[] = { "SCALAR", "VEC2", "VEC3", "VEC4", "MAT2", "MAT3", "MAT4" };

struct Accessor
{
	uint32 bufferView;
	sint32 byteOffset;
	ComponentType componentType;
	size_t count;
	AccessorType type;
};

struct Image
{
	std::string uri;
};

struct TextureInfo
{
	size_t index;
};
struct Texture
{
	size_t source;
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
};
enum AlphaMode
{
	OPAQUE,
	MASK,
	BLEND
};
const char *AlphaModeNames[] = { "OPAQUE", "MASK", "BLEND" };

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
};

template <class T, class Allocator>
void write(FILE *file, std::vector<T, Allocator> &cont)
{

	fprintf(file, "[");
	auto len = cont.size();

	auto it = cont.begin();
	for (auto i = 0; i < len; i++, ++it)
	{
		if (i > 0)
		{
			fprintf(file, ",");
		}
		write(file, *it);
	}
	fprintf(file, "]");
}

void write(FILE *file, const Accessor &object)
{
	fprintf(file, R"({ "bufferView": %i, "byteOffset": %i, "componentType": %i, "count": %lu, "type": "%s" })", object.bufferView, object.byteOffset, object.componentType, object.count, AccessorTypeNames[object.type]);
}

void write(FILE *file, const TextureInfo &object)
{
	fprintf(file, R"({ "index": %lu })", object.index);
}

void write(FILE *file, const MetallicRoughness &object)
{
	fprintf(file, R"({ "baseColorTexture": )");
	write(file, object.baseColorTexture);
	fprintf(file, R"(})");
}

void write(FILE *file, const Material &object)
{
	fprintf(file, R"({ "alphaMode": "%s", "pbrMetallicRoughness": )", AlphaModeNames[object.alphaMode]);
	write(file, object.pbrMetallicRoughness);
	fprintf(file, R"(})");
}

void write(FILE *file, const Texture &object)
{
	fprintf(file, R"({ "source": %lu })", object.source);
}

void write(FILE *file, const Image &object)
{
	fprintf(file, R"({ "uri": "%s" })", object.uri.c_str());
}

}

#endif //GLTF_H
