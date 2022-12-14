
add_output_files(workspace_qml ${CMAKE_CURRENT_SOURCE_DIR}/qml ${MODULE_DEPLOY_PATH}/workspace workspace_qml_sources)

add_custom_target(workspace
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    SOURCES ${workspace_qml_sources}
)



add_subdirectory(builder)
add_subdirectory(icons)
add_subdirectory(nodeeditor)
add_subdirectory(quickqanava)
