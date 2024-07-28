add_library(libgltf-convert-zone)
add_library(libgltf-convert::zone ALIAS libgltf-convert-zone)

target_link_libraries(libgltf-convert-zone PUBLIC nel3d nelmisc nelligo gltf::common)
target_sources(libgltf-convert-zone
        PRIVATE
        gltf-convert/zone/utils.cpp
)
target_sources(libgltf-convert-zone
        PUBLIC
        FILE_SET HEADERS
        FILES
        gltf-convert/zone/utils.h
)

add_executable(gltf-convert-zone)
target_link_libraries(gltf-convert-zone PRIVATE libgltf-convert-zone)
target_sources(gltf-convert-zone
        PRIVATE
        gltf-convert/zone/main.cpp
)

NL_DEFAULT_PROPS(gltf-convert-zone "NeL, Tools, 3D: gltf-convert-zone")
NL_ADD_RUNTIME_FLAGS(gltf-convert-zone)

INSTALL(TARGETS gltf-convert-zone RUNTIME DESTINATION ${NL_BIN_PREFIX} COMPONENT tools3d)


add_test_executable(gltf-convert-zone-utils_test
        tests/utils.test.cpp
)
target_link_libraries(gltf-convert-zone-utils_test
        libgltf-convert::zone
)

