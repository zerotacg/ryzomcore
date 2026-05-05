#ifndef LIBGLTF_GLTF_H
#define LIBGLTF_GLTF_H

#include <libgltf/Accessor.h>
#include <libgltf/Asset.h>
#include <libgltf/Attributes.h>
#include <libgltf/Buffer.h>
#include <libgltf/BufferView.h>
#include <libgltf/Image.h>
#include <libgltf/Info.h>
#include <libgltf/JsonWriter.h>
#include <libgltf/Material.h>
#include <libgltf/Mesh.h>
#include <libgltf/Primitive.h>
#include <libgltf/Texture.h>

namespace gltf {

inline const Info info = { .version = "2.0" };

}

#endif // LIBGLTF_GLTF_H
