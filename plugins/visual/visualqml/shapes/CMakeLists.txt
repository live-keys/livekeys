
add_library(shapes SHARED)

set_target_properties(shapes PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/visual/shapes"
    RUNTIME_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/visual/shapes"
)

if(APPLE)
    set_target_properties(shapes PROPERTIES LINK_FLAGS
        "-Wl,-rpath,@executable_path/../Link -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@executable_path/../Frameworks/Live.framework/Libraries")
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(shapes PROPERTIES LINK_FLAGS
        "-Wl,-rpath,$$ORIGIN/../../../link"
    )
endif()


add_output_files(visual_shapes_qml        ${CMAKE_CURRENT_SOURCE_DIR}/qml        ${MODULE_DEPLOY_PATH}/visual/shapes          visual_shapes_qml_sources)

target_sources(shapes PRIVATE ${visual_shapes_qml})


target_sources(shapes PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/shapes_plugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/svgview.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/triangle.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/linegrid.cpp"
)

target_include_directories(shapes
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_compile_definitions(shapes PRIVATE LV_SHAPES_PLUGIN)

target_link_libraries(shapes
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::Svg )



