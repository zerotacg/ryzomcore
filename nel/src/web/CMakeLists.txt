FILE(GLOB SRC *.cpp *.h)
FILE(GLOB HEADERS ../../include/nel/web/*.h)

SOURCE_GROUP("" FILES ${SRC} ${HEADERS})

NL_TARGET_LIB(nelweb ${HEADERS} ${SRC})
add_library(NeL::web ALIAS nelweb)
set_target_properties(nelweb PROPERTIES EXPORT_NAME web)

TARGET_LINK_LIBRARIES(nelweb
        PUBLIC
        NeL::misc
        PRIVATE
        CURL::libcurl
        OpenSSL::SSL
        OpenSSL::Crypto
        LibXml2::LibXml2
)

NL_DEFAULT_PROPS(nelweb "NeL, Library: NeL Web")
NL_ADD_RUNTIME_FLAGS(nelweb)

NL_ADD_LIB_SUFFIX(nelweb)

IF(WITH_PCH)
  target_precompile_headers(nelweb PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/stdweb.h)
ENDIF()

NL_GEN_PC(nel-web.pc)

IF(WITH_INSTALL_LIBRARIES OR NOT WITH_STATIC)
  INSTALL(TARGETS nelweb
          EXPORT "${PROJECT_NAME}-web-targets"
          COMPONENT libraries
          LIBRARY DESTINATION ${NL_LIB_PREFIX}
          ARCHIVE DESTINATION ${NL_LIB_PREFIX}
  )
ENDIF()

IF(WITH_INSTALL_LIBRARIES)
  install(EXPORT "${PROJECT_NAME}-web-targets"
          COMPONENT libraries
          NAMESPACE "${PROJECT_NAME}::"
          DESTINATION "${config_install_dir}"
  )
ENDIF()
