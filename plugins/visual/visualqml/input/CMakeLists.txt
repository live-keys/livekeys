
add_output_files(visual_input_qml ${CMAKE_CURRENT_SOURCE_DIR}/qml ${MODULE_DEPLOY_PATH}/visual/input visual_input_qml_sources)

add_custom_target(visual_input
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    SOURCES ${visual_input_qml_sources}
)

