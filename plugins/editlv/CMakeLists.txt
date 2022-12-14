
add_library(editlv SHARED)

set_target_properties(editlv PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/editlv"
)

if(APPLE)
    set_target_properties(editlv PROPERTIES LINK_FLAGS
        "-Wl,-rpath,@executable_path/../Link -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@executable_path/../Frameworks/Live.framework/Libraries")
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(editlv PROPERTIES LINK_FLAGS
        "-Wl,-rpath,$$ORIGIN/../../link"
    )
endif()


add_output_files(editlv_qml        ${CMAKE_CURRENT_SOURCE_DIR}/qml        ${MODULE_DEPLOY_PATH}/editlv          editlv_qml_sources)

target_sources(editlv PRIVATE ${editlv_qml_sources})

target_sources(editlv PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/bindingchannel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/bindingspan.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/bindingspanmodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/codeconverter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/declaration.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/documenttree.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/editlv_plugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/editlvsettings.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/languagelvextension.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/languagelvhandler.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/languagelvhighlighter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lvaddcontainer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lvcomponentmodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lveditfragment.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lvpropertymodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/projectlvmonitor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/projectlvscanner.cpp"
)

target_include_directories(editlv
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_compile_definitions(editlv PRIVATE LV_EDITLV_PLUGIN)

target_link_libraries(editlv
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick )

target_link_libraries(editlv PRIVATE lvbase lvview lveditor lvelements)
