ADD_EXECUTABLE(anim_to_gltf)
target_sources(anim_to_gltf
        PRIVATE
        main.cpp
        mapper/TrackMapper.cpp
        mapper/TrackDefaultQuatMapper.cpp
        mapper/TrackSampledVectorMapper.cpp
        mapper/TrackSampledQuatMapper.cpp
)
target_sources(anim_to_gltf
        PRIVATE
        FILE_SET HEADERS
        FILES
        common.h
        SampleData.h
        mapper/TrackMapper.h
        mapper/TrackDefaultQuatMapper.h
        mapper/TrackSampledVectorMapper.h
        mapper/TrackSampledQuatMapper.h
)

TARGET_LINK_LIBRARIES(anim_to_gltf nel3d nelmisc gltf::common)
NL_DEFAULT_PROPS(anim_to_gltf "NeL, Tools, 3D: anim_to_gltf")
NL_ADD_RUNTIME_FLAGS(anim_to_gltf)

INSTALL(TARGETS anim_to_gltf RUNTIME DESTINATION ${NL_BIN_PREFIX} COMPONENT tools3d)
