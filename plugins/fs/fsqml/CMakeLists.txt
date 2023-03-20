
add_library(fs SHARED)

set_target_properties(fs PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/fs"
    RUNTIME_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/fs"
)

if(APPLE)
    set_target_properties(fs PROPERTIES LINK_FLAGS
        "-Wl,-rpath,@executable_path/../Link -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@executable_path/../Frameworks/Live.framework/Libraries")
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(fs PROPERTIES LINK_FLAGS
        "-Wl,-rpath,$$ORIGIN/../../link"
    )
endif()


add_output_files(fs_qml      ${CMAKE_CURRENT_SOURCE_DIR}/qml            ${MODULE_DEPLOY_PATH}/fs          fs_qml_sources)
add_output_files(fs_palettes ${CMAKE_CURRENT_SOURCE_DIR}/qml/palettes   ${MODULE_DEPLOY_PATH}/fs/palettes fs_palettes_sources)

target_sources(fs PRIVATE ${fs_qml_sources})
target_sources(fs PRIVATE ${fs_palettes_sources})


if(BUILD_PACKAGE_SAMPLES)
    add_output_files(fs_sample_fileexplorer ${CMAKE_CURRENT_SOURCE_DIR}/samples/fileexplorer ${MODULE_DEPLOY_PATH}/fs/samples/fileexplorer     fs_sample_fileexplorer_sources)
    add_output_files(fs_sample_fileexplorer_res ${CMAKE_CURRENT_SOURCE_DIR}/samples/fileexplorer/resources ${MODULE_DEPLOY_PATH}/fs/samples/fileexplorer/resources     fs_sample_fileexplorer_res_sources)
    target_sources(fs PRIVATE ${fs_sample_fileexplorer_sources})
    target_sources(fs PRIVATE ${fs_sample_fileexplorer_res_sources})

    add_output_files(fs_sample_readingfiles ${CMAKE_CURRENT_SOURCE_DIR}/samples/readingfiles ${MODULE_DEPLOY_PATH}/fs/samples/readingfiles     fs_sample_readingfiles_sources)
    add_output_files(fs_sample_readingfiles_res ${CMAKE_CURRENT_SOURCE_DIR}/samples/readingfiles/resources ${MODULE_DEPLOY_PATH}/fs/samples/readingfiles/resources     fs_sample_readingfiles_res_sources)
    target_sources(fs PRIVATE ${fs_sample_readingfiles_sources})
    target_sources(fs PRIVATE ${fs_sample_readingfiles_res_sources})
endif()

target_sources(fs PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/fs_plugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/listdir.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmldir.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmldirectorytostream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlfile.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlfilereader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlfiledescriptor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlfilestream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlpath.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlurlinfo.cpp"
)

target_include_directories(fs
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_compile_definitions(fs PRIVATE LV_FS_PLUGIN)

target_link_libraries(fs
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick )

target_link_libraries(fs PRIVATE lvbase lvview lveditor)
