
add_library(lcvimgproc SHARED)

set_target_properties(lcvimgproc PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/lcvimgproc"
)

if(APPLE)
    set_target_properties(lcvimgproc PROPERTIES LINK_FLAGS
        "-Wl,-rpath,@executable_path/../Link -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@executable_path/../Frameworks/Live.framework/Libraries")
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(lcvimgproc PROPERTIES LINK_FLAGS
        "-Wl,-rpath,$$ORIGIN/../../link"
    )
endif()


add_output_files(lcvimgproc_qml        ${CMAKE_CURRENT_SOURCE_DIR}/qml        ${MODULE_DEPLOY_PATH}/lcvimgproc          lcvimgproc_qml_sources)

target_sources(lcvimgproc PRIVATE ${lcvimgproc_qml_sources})

target_sources(lcvimgproc PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/colorspace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lcvimgproc_plugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qcascadeclassifier.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qfeaturedetection.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qfilteringoperations.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qprojectionmapper.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qprojectionsurface.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qshapedescriptors.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qtransformations.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qtransformimage.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qcachedwarpperspective.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qdraw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qgeometry.cpp"
)

target_include_directories(lcvimgproc
    PRIVATE ${OpenCV_INCLUDE_DIRS}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_compile_definitions(lcvimgproc PRIVATE Q_LCV)

target_link_libraries(lcvimgproc
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick ${OpenCV_LIBS})

target_link_libraries(lcvimgproc PRIVATE lvbase lvview lveditor lveditqmljs base lcvcore)
