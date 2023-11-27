FILE(GLOB SRC *.cpp *.h *.rc)

SOURCE_GROUP("" FILES ${SRC})

ADD_EXECUTABLE(zone_to_gltf ${SRC})

TARGET_LINK_LIBRARIES(zone_to_gltf nel3d nelmisc nelligo)
NL_DEFAULT_PROPS(zone_to_gltf "NeL, Tools, 3D: zone_to_gltf")
NL_ADD_RUNTIME_FLAGS(zone_to_gltf)

INSTALL(TARGETS zone_to_gltf RUNTIME DESTINATION ${NL_BIN_PREFIX} COMPONENT tools3d)
