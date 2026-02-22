FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(nl_sample_nelvp WIN32 ${SRC})

TARGET_LINK_LIBRARIES(nl_sample_nelvp nelmisc nel3d)
NL_DEFAULT_PROPS(nl_sample_nelvp "NeL, Samples, 3D: nelvp")
NL_ADD_RUNTIME_FLAGS(nl_sample_nelvp)

INSTALL(TARGETS nl_sample_nelvp RUNTIME DESTINATION ${NL_BIN_PREFIX} COMPONENT samples3d)
