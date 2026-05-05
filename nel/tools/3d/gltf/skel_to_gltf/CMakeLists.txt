FILE(GLOB SRC *.cpp *.h)

SOURCE_GROUP("" FILES ${SRC})

ADD_EXECUTABLE(skel_to_gltf ${SRC})

TARGET_LINK_LIBRARIES(skel_to_gltf nel3d nelmisc gltf::common)
NL_DEFAULT_PROPS(skel_to_gltf "NeL, Tools, 3D: skel_to_gltf")
NL_ADD_RUNTIME_FLAGS(skel_to_gltf)

INSTALL(TARGETS skel_to_gltf RUNTIME DESTINATION ${NL_BIN_PREFIX} COMPONENT tools3d)
