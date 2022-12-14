
add_output_files(workspace_icons_qml ${CMAKE_CURRENT_SOURCE_DIR}/qml ${MODULE_DEPLOY_PATH}/workspace/icons workspace_icons_qml_sources)

add_custom_target(workspace_icons
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    SOURCES ${workspace_icons_qml_sources}
)

