FILE(GLOB SRC *.cpp *.h *.rc)

SOURCE_GROUP("" FILES ${SRC})

ADD_EXECUTABLE(shape_to_gltf ${SRC})

TARGET_LINK_LIBRARIES(shape_to_gltf nel3d nelmisc gltf::common)
NL_DEFAULT_PROPS(shape_to_gltf "NeL, Tools, 3D: shape_to_gltf")
NL_ADD_RUNTIME_FLAGS(shape_to_gltf)

INSTALL(TARGETS shape_to_gltf RUNTIME DESTINATION ${NL_BIN_PREFIX} COMPONENT tools3d)
