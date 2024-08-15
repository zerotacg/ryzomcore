#ifndef LIBGLTF_ACCESSOR_H
#define LIBGLTF_ACCESSOR_H

#include <nel/misc/types_nl.h>

#include <libgltf/other.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct Accessor
{
	size_t bufferView;
	sint32 byteOffset;
	ComponentType componentType;
	bool normalized;
	size_t count;
	AccessorType type;

	void write(JsonWriter &writer) const;

	static Accessor position(uint32 bufferView, sint32 byteOffset, size_t count)
	{
		return Accessor {
			.bufferView = bufferView,
			.byteOffset = byteOffset,
			.componentType = ComponentType::FLOAT,
			.count = count,
			.type = AccessorType::VEC3
		};
	}

	static Accessor normal(uint32 bufferView, sint32 byteOffset, size_t count)
	{
		return Accessor {
			.bufferView = bufferView,
			.byteOffset = byteOffset,
			.componentType = ComponentType::FLOAT,
			.count = count,
			.type = AccessorType::VEC3
		};
	}

	static Accessor weight(uint32 bufferView, sint32 byteOffset, size_t count)
	{
		return Accessor {
			.bufferView = bufferView,
			.byteOffset = byteOffset,
			.componentType = ComponentType::FLOAT,
			.count = count,
			.type = AccessorType::VEC4
		};
	}

	static Accessor joint(uint32 bufferView, sint32 byteOffset, size_t count)
	{
		return Accessor {
			.bufferView = bufferView,
			.byteOffset = byteOffset,
			.componentType = ComponentType::UNSIGNED_BYTE,
			.count = count,
			.type = AccessorType::VEC4
		};
	}
};

}

#endif // LIBGLTF_ACCESSOR_H
