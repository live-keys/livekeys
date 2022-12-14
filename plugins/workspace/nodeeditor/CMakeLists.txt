
add_output_files(workspace_nodeeditor_qml ${CMAKE_CURRENT_SOURCE_DIR}/qml ${MODULE_DEPLOY_PATH}/workspace/nodeeditor workspace_nodeeditor_qml_sources)

add_custom_target(workspace_nodeeditor
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    SOURCES ${workspace_nodeeditor_qml_sources}
)

