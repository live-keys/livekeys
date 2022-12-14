
add_library(lcvphoto SHARED)

set_target_properties(lcvphoto PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/lcvphoto"
)

if(APPLE)
    set_target_properties(lcvphoto PROPERTIES LINK_FLAGS
        "-Wl,-rpath,@executable_path/../Link -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@executable_path/../Frameworks/Live.framework/Libraries")
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(lcvphoto PROPERTIES LINK_FLAGS
        "-Wl,-rpath,$$ORIGIN/../../link"
    )
endif()


add_output_files(lcvphoto_qml        ${CMAKE_CURRENT_SOURCE_DIR}/qml        ${MODULE_DEPLOY_PATH}/lcvphoto          lcvphoto_qml_sources)
add_output_files(lcvphoto_palettes   ${CMAKE_CURRENT_SOURCE_DIR}/palettes   ${MODULE_DEPLOY_PATH}/lcvphoto/palettes lcvphoto_palettes_sources)

target_sources(lcvphoto PRIVATE ${lcvphoto_qml_sources})
target_sources(lcvphoto PRIVATE ${lcvphoto_palettes_sources})


target_sources(lcvphoto PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lcvphoto_plugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qadjustments.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qalignmtb.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qcalibratedebevec.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qcalibraterobertson.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmergedebevec.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmergerobertson.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qtonemap.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qtonemapdrago.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qtonemapmantiuk.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qstitcher.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qdenoising.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qtonemapreinhard.cpp"
)

target_include_directories(lcvphoto
    PRIVATE ${OpenCV_INCLUDE_DIRS}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_compile_definitions(lcvphoto PRIVATE Q_LCV)

target_link_libraries(lcvphoto
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick ${OpenCV_LIBS})

target_link_libraries(lcvphoto PRIVATE lvbase lvview lveditor lveditqmljs base lcvcore)
