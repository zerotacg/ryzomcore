set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

ADD_LIBRARY(gltf)

ADD_LIBRARY(gltf::common ALIAS gltf)
target_sources(gltf
        PRIVATE
        libgltf/Accessor.cpp
        libgltf/Asset.cpp
)
target_sources(gltf
        PUBLIC
        FILE_SET HEADERS
        FILES
        libgltf/Accessor.h
        libgltf/Animation.h
        libgltf/Asset.h
        libgltf/Attributes.h
        libgltf/Buffer.h
        libgltf/BufferView.h
        libgltf/Channel.h
        libgltf/ChannelTarget.h
        libgltf/ChannelTargetPath.h
        libgltf/Image.h
        libgltf/Info.h
        libgltf/Interpolation.h
        libgltf/JsonWriter.h
        libgltf/Material.h
        libgltf/Mesh.h
        libgltf/Node.h
        libgltf/Primitive.h
        libgltf/Sampler.h
        libgltf/Scene.h
        libgltf/Skin.h
        libgltf/Texture.h
        libgltf/gltf.h
        libgltf/other.h
)

TARGET_INCLUDE_DIRECTORIES(gltf
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)
