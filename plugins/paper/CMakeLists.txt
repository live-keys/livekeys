
add_library(paper SHARED)

set_target_properties(paper PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/paper"
)

if(APPLE)
    set_target_properties(paper PROPERTIES LINK_FLAGS
        "-Wl,-rpath,@executable_path/../Link -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@executable_path/../Frameworks/Live.framework/Libraries")
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(paper PROPERTIES LINK_FLAGS
        "-Wl,-rpath,$$ORIGIN/../../link"
    )
endif()


add_output_files(paper_qml              ${CMAKE_CURRENT_SOURCE_DIR}/qml                    ${MODULE_DEPLOY_PATH}/paper                       paper_qml_sources)
add_output_files(paper_tools_qml        ${CMAKE_CURRENT_SOURCE_DIR}/qml/tools              ${MODULE_DEPLOY_PATH}/paper/tools                 paper_tools_qml_sources)
add_output_files(paper_3p_js            ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/paperqml/js   ${MODULE_DEPLOY_PATH}/paper/paperjs               paper_3p_js_sources)
add_output_files(paper_3p_qml           ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/paperqml/qml  ${MODULE_DEPLOY_PATH}/paper/paperqml              paper_3p_qml_sources)
add_output_files(paper_3p_fonts         ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/samples/fonts ${MODULE_DEPLOY_PATH}/paper/fonts                 paper_3p_fonts_sources)
add_output_files(papergrapher_js        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/js            ${MODULE_DEPLOY_PATH}/paper/papergrapherjs        papergrapher_js_sources)
add_output_files(papergrapher_qml       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qml           ${MODULE_DEPLOY_PATH}/paper/papergrapherqml       papergrapher_qml_sources)
add_output_files(papergrapher_tools_qml ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qml/tools     ${MODULE_DEPLOY_PATH}/paper/papergrapherqml/tools papergrapher_tools_qml_sources)

target_sources(paper PRIVATE ${paper_qml_sources})
target_sources(paper PRIVATE ${paper_tools_qml_sources})
target_sources(paper PRIVATE ${paper_3p_js_sources})
target_sources(paper PRIVATE ${paper_3p_qml_sources})
target_sources(paper PRIVATE ${paper_3p_fonts_sources})
target_sources(paper PRIVATE ${papergrapher_js_sources})
target_sources(paper PRIVATE ${papergrapher_qml_sources})
target_sources(paper PRIVATE ${papergrapher_tools_qml_sources})


target_sources(paper PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/paperqml/src/jsmodule.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/paper_plugin.cpp"
)

target_include_directories(paper
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_compile_definitions(paper PRIVATE LV_PAPER_LIB)

target_link_libraries(paper
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick )

target_link_libraries(paper PRIVATE lvbase lvview)
