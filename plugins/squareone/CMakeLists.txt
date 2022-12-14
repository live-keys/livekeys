
add_output_files(squareone_qml ${CMAKE_CURRENT_SOURCE_DIR}/qml ${MODULE_DEPLOY_PATH}/squareone squareone_qml_sources)

add_output_files(
    tutorials_firstapplication_files
    ${CMAKE_CURRENT_SOURCE_DIR}/tutorials/firstapplication
    ${MODULE_DEPLOY_PATH}/squareone/tutorials/firstapplication
    tutorials_firstapplication_files_sources
)
add_output_files(
    tutorials_firstapplication_resources
    ${CMAKE_CURRENT_SOURCE_DIR}/tutorials/firstapplication/resources
    ${MODULE_DEPLOY_PATH}/squareone/tutorials/firstapplication/resources
    tutorials_firstapplication_resources_sources
)

add_output_files(
    tutorials_workspace_files
    ${CMAKE_CURRENT_SOURCE_DIR}/tutorials/workspace
    ${MODULE_DEPLOY_PATH}/squareone/tutorials/workspace
    tutorials_workspace_files_sources
)
add_output_files(
    tutorials_workspace_resources
    ${CMAKE_CURRENT_SOURCE_DIR}/tutorials/workspace/resources
    ${MODULE_DEPLOY_PATH}/squareone/tutorials/workspace/resources
    tutorials_workspace_resources_sources
)

add_custom_target(squareone
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    SOURCES
        ${squareone_qml_sources}
        ${tutorials_firstapplication_files_sources}
        ${tutorials_firstapplication_resources_sources}
        ${tutorials_workspace_files_sources}
        ${tutorials_workspace_resources_sources}
)
