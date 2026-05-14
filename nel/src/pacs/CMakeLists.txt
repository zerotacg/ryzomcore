FILE(GLOB SRC *.cpp *.h)
FILE(GLOB HEADERS ../../include/nel/pacs/*.h)

NL_TARGET_LIB(nelpacs ${HEADERS} ${SRC})
add_library(NeL::pacs ALIAS nelpacs)
set_target_properties(nelpacs PROPERTIES EXPORT_NAME pacs)

TARGET_LINK_LIBRARIES(nelpacs NeL::net)
NL_DEFAULT_PROPS(nelpacs "NeL, Library: NeL PACS")
NL_ADD_RUNTIME_FLAGS(nelpacs)

NL_ADD_LIB_SUFFIX(nelpacs)

IF(WITH_PCH)
  target_precompile_headers(nelpacs PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/stdpacs.h)
ENDIF()

NL_GEN_PC(nel-pacs.pc)

IF(WITH_INSTALL_LIBRARIES OR NOT WITH_STATIC)
  INSTALL(TARGETS nelpacs
          EXPORT "${PROJECT_NAME}-pacs-targets"
          COMPONENT libraries
          LIBRARY DESTINATION ${NL_LIB_PREFIX}
          ARCHIVE DESTINATION ${NL_LIB_PREFIX}
  )
ENDIF()

IF(WITH_INSTALL_LIBRARIES)
  install(EXPORT "${PROJECT_NAME}-pacs-targets"
          COMPONENT libraries
          NAMESPACE "${PROJECT_NAME}::"
          DESTINATION "${config_install_dir}"
  )
ENDIF()
