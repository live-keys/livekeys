
add_library(editor SHARED)

set_target_properties(editor PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/editor"
    RUNTIME_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/editor"
)

if(APPLE)
    set_target_properties(editor PROPERTIES LINK_FLAGS
        "-Wl,-rpath,@executable_path/../Link -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@executable_path/../Frameworks/Live.framework/Libraries")
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(editor PROPERTIES LINK_FLAGS
        "-Wl,-rpath,$$ORIGIN/../../link"
    )
endif()


add_output_files(editor_qml        ${CMAKE_CURRENT_SOURCE_DIR}/qml        ${MODULE_DEPLOY_PATH}/editor          editor_qml_sources)
add_output_files(editor_qml_styles ${CMAKE_CURRENT_SOURCE_DIR}/qml/style  ${MODULE_DEPLOY_PATH}/editor/style    editor_qml_style_sources)
add_output_files(editor_palettes   ${CMAKE_CURRENT_SOURCE_DIR}/palettes   ${MODULE_DEPLOY_PATH}/editor/palettes editor_palettes_sources)

target_sources(editor PRIVATE ${editor_qml_sources})
target_sources(editor PRIVATE ${editor_qml_style_sources})
target_sources(editor PRIVATE ${editor_palettes_sources})


target_sources(editor PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/editor_plugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/textsearch.cpp"
)

target_include_directories(editor
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_compile_definitions(editor PRIVATE LV_EDITOR_PLUGIN)

target_link_libraries(editor
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick )

target_link_libraries(editor PRIVATE lvbase lvview lveditor)
