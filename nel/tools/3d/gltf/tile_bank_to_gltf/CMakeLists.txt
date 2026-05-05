FILE(GLOB SRC *.cpp *.h *.rc)

SOURCE_GROUP("" FILES ${SRC})

ADD_EXECUTABLE(tile_bank_to_gltf ${SRC})

TARGET_LINK_LIBRARIES(tile_bank_to_gltf nel3d nelmisc gltf::common)
NL_DEFAULT_PROPS(tile_bank_to_gltf "NeL, Tools, 3D: zone_to_gltf")
NL_ADD_RUNTIME_FLAGS(tile_bank_to_gltf)

INSTALL(TARGETS tile_bank_to_gltf RUNTIME DESTINATION ${NL_BIN_PREFIX} COMPONENT tools3d)
