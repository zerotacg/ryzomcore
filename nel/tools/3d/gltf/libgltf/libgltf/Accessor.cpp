#include <libgltf/Accessor.h>

namespace gltf {

void Accessor::write(JsonWriter &writer) const
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

}
