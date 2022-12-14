
function(add_output_files target_name from_path to_path target_sources)

    list(APPEND ADD_DEPENDENCIES_OUTPUT "")
    list(APPEND ADD_DEPENDENCIES_SOURCE "")

    file(GLOB FILES_TO_ADD LIST_DIRECTORIES false ${from_path}/*)
    foreach(FILE_TO_ADD IN LISTS FILES_TO_ADD)
        get_filename_component(FILE_TO_ADD_NAME ${FILE_TO_ADD} NAME)
        add_custom_command(
            OUTPUT ${to_path}/${FILE_TO_ADD_NAME}
            DEPENDS ${FILE_TO_ADD}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FILE_TO_ADD} "${to_path}/${FILE_TO_ADD_NAME}"
        )

        list(APPEND ADD_DEPENDENCIES_OUTPUT "${to_path}/${FILE_TO_ADD_NAME}")
        list(APPEND ADD_DEPENDENCIES_SOURCE "${FILE_TO_ADD}")
    endforeach()

    add_custom_target(${target_name} ALL DEPENDS ${ADD_DEPENDENCIES_OUTPUT})

    set(${target_sources} ${ADD_DEPENDENCIES_SOURCE} PARENT_SCOPE)

endfunction()
