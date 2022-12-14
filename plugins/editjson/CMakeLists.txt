
add_library(editjson SHARED)

set_target_properties(editjson PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/editjson"
)

if(APPLE)
    set_target_properties(editjson PROPERTIES LINK_FLAGS
        "-Wl,-rpath,@executable_path/../Link -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@executable_path/../Frameworks/Live.framework/Libraries")
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(editjson PROPERTIES LINK_FLAGS
        "-Wl,-rpath,$$ORIGIN/../../link"
    )
endif()

add_output_files(editjson_qml        ${CMAKE_CURRENT_SOURCE_DIR}/qml        ${MODULE_DEPLOY_PATH}/qml          editjson_qml_sources)

target_sources(editjson PRIVATE ${editjson_qml_sources})

target_sources(editjson PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/editjson_plugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/editjsonobject.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmllanguageobject.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/queryhighlighter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlscriptlanguagehandler.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/treesitterjson/src/parser.c"
)

target_include_directories(editjson
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/treesitterjson/src"
)
target_compile_definitions(editjson PRIVATE LV_EDITJSON_PLUGIN)

target_link_libraries(editjson
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick )

target_link_libraries(editjson PRIVATE lvbase lvview lveditor lvelements)
