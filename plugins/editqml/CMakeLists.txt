
add_library(editqml SHARED)

set_target_properties(editqml PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/editqml"
)

if(APPLE)
    set_target_properties(editqml PROPERTIES LINK_FLAGS
        "-Wl,-rpath,@executable_path/../Link -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@executable_path/../Frameworks/Live.framework/Libraries")
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(editqml PROPERTIES LINK_FLAGS
        "-Wl,-rpath,$$ORIGIN/../../link"
    )
endif()


add_output_files(editqml_qml        ${CMAKE_CURRENT_SOURCE_DIR}/qml        ${MODULE_DEPLOY_PATH}/editqml          editqml_qml_sources)

target_sources(editqml PRIVATE ${editqml_qml_sources})

target_sources(editqml PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/editqml_plugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmldocumentsyntax.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmldocumentsyntaxvalue.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlplugininfoextractor.cpp"
)

target_include_directories(editqml
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_compile_definitions(editqml PRIVATE LV_EDITQML_PLUGIN)

target_link_libraries(editqml
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick )

target_link_libraries(editqml PRIVATE lvbase lvview lveditor lveditqmljs)
