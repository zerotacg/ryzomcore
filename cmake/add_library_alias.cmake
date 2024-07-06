function(add_library_project_alias target alias)
    set_target_properties(${target} PROPERTIES EXPORT_NAME "${alias}")

    add_library(${PROJECT_NAME}::${alias} ALIAS ${target})
endfunction()
