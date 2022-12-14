
add_library(lcvfeatures2d SHARED)

set_target_properties(lcvfeatures2d PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/lcvfeatures2d"
)

if(APPLE)
    set_target_properties(lcvfeatures2d PROPERTIES LINK_FLAGS
        "-Wl,-rpath,@executable_path/../Link -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@executable_path/../Frameworks/Live.framework/Libraries")
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(lcvfeatures2d PROPERTIES LINK_FLAGS
        "-Wl,-rpath,$$ORIGIN/../../link"
    )
endif()


add_output_files(lcvfeatures2d_qml        ${CMAKE_CURRENT_SOURCE_DIR}/qml        ${MODULE_DEPLOY_PATH}/lcvfeatures2d          lcvfeatures2d_qml_sources)

target_sources(lcvfeatures2d PRIVATE ${lcvfeatures2d_qml_sources})


target_sources(lcvfeatures2d PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lcvfeatures2d_plugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qbriskdescriptorextractor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qbriskfeaturedetector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qbruteforcematcher.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qdescriptorextractor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qdescriptormatcher.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qdescriptormatchfilter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qdmatchvector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qdrawmatches.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qfastfeaturedetector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qfeaturedetector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qflannbasedmatcher.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qkeypoint.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qkeypointvector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmserfeaturedetector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qorbdescriptorextractor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qorbfeaturedetector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qsimpleblobdetector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmatchestolocalkeypoint.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qkeypointtoscenemap.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qkeypointhomography.cpp"
)

target_include_directories(lcvfeatures2d
    PRIVATE ${OpenCV_INCLUDE_DIRS}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_definitions(lcvfeatures2d PRIVATE Q_LCVFEATURES2D_LIB Q_LCV)

target_link_libraries(lcvfeatures2d
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick ${OpenCV_LIBS})

target_link_libraries(lcvfeatures2d PRIVATE lvbase lvview lveditor lveditqmljs base live lcvcore)
