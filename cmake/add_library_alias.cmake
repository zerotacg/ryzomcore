function(add_library_project_alias target alias)
    string(TOLOWER ${PROJECT_NAME} _LOWER_PROJECT_NAME)
    set_target_properties(${target} PROPERTIES EXPORT_NAME "${alias}")

    add_library(${PROJECT_NAME}::${alias} ALIAS ${target})
endfunction()
