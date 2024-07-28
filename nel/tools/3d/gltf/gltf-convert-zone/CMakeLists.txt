ADD_EXECUTABLE(gltf-convert-zone)

TARGET_LINK_LIBRARIES(gltf-convert-zone nel3d nelmisc nelligo gltf::common)
target_sources(gltf-convert-zone
        PRIVATE
        gltf-convert/zone/main.cpp
)
target_sources(gltf-convert-zone
        PRIVATE
        FILE_SET HEADERS
        FILES
)


NL_DEFAULT_PROPS(gltf-convert-zone "NeL, Tools, 3D: gltf-convert-zone")
NL_ADD_RUNTIME_FLAGS(gltf-convert-zone)

INSTALL(TARGETS gltf-convert-zone RUNTIME DESTINATION ${NL_BIN_PREFIX} COMPONENT tools3d)
