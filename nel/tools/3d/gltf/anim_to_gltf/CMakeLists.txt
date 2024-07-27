ADD_EXECUTABLE(anim_to_gltf)
target_sources(anim_to_gltf
        PRIVATE
        main.cpp
        mapper/TrackMapper.cpp
        mapper/TrackDefaultQuatMapper.cpp
        mapper/TrackDefaultVectorMapper.cpp
        mapper/TrackSampledQuatMapper.cpp
        mapper/TrackSampledVectorMapper.cpp
)
target_sources(anim_to_gltf
        PRIVATE
        FILE_SET HEADERS
        FILES
        common.h
        ChannelData.h
        mapper/TrackMapper.h
        mapper/TrackDefaultQuatMapper.h
        mapper/TrackDefaultVectorMapper.h
        mapper/TrackSampledQuatMapper.h
        mapper/TrackSampledVectorMapper.h
)

TARGET_LINK_LIBRARIES(anim_to_gltf nel3d nelmisc gltf::common)
NL_DEFAULT_PROPS(anim_to_gltf "NeL, Tools, 3D: anim_to_gltf")
NL_ADD_RUNTIME_FLAGS(anim_to_gltf)

INSTALL(TARGETS anim_to_gltf RUNTIME DESTINATION ${NL_BIN_PREFIX} COMPONENT tools3d)
