
add_library(lcvvideo SHARED)

set_target_properties(lcvvideo PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/lcvvideo"
)

if(APPLE)
    set_target_properties(lcvvideo PROPERTIES LINK_FLAGS
        "-Wl,-rpath,@executable_path/../Link -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@executable_path/../Frameworks/Live.framework/Libraries")
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(lcvvideo PROPERTIES LINK_FLAGS
        "-Wl,-rpath,$$ORIGIN/../../link"
    )
endif()


add_output_files(lcvvideo_qml        ${CMAKE_CURRENT_SOURCE_DIR}/qml        ${MODULE_DEPLOY_PATH}/lcvvideo          lcvvideo_qml_sources)
add_output_files(lcvvideo_palettes   ${CMAKE_CURRENT_SOURCE_DIR}/palettes   ${MODULE_DEPLOY_PATH}/lcvvideo/palettes lcvvideo_palettes_sources)

target_sources(lcvvideo PRIVATE ${lcvvideo_qml_sources})
target_sources(lcvvideo PRIVATE ${lcvvideo_palettes_sources})


target_sources(lcvvideo PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lcvvideo_plugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qcalcopticalflowpyrlk.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qbackgroundsubtractor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qbackgroundsubtractormog2.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qbackgroundsubtractorknn.cpp"
)

target_include_directories(lcvvideo
    PRIVATE ${OpenCV_INCLUDE_DIRS}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_compile_definitions(lcvvideo PRIVATE Q_LCV)

target_link_libraries(lcvvideo
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick ${OpenCV_LIBS})

target_link_libraries(lcvvideo PRIVATE lvbase lvview lveditor lveditqmljs base lcvcore)
