
add_library(table SHARED)

set_target_properties(table PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/table"
    RUNTIME_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/table"
)

if(APPLE)
    set_target_properties(table PROPERTIES LINK_FLAGS
        "-Wl,-rpath,@executable_path/../Link -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@executable_path/../Frameworks/Live.framework/Libraries")
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(table PROPERTIES LINK_FLAGS
        "-Wl,-rpath,$$ORIGIN/../../link"
    )
endif()


add_output_files(table_qml        ${CMAKE_CURRENT_SOURCE_DIR}/qml        ${MODULE_DEPLOY_PATH}/table          table_qml_sources)

target_sources(table PRIVATE ${table_qml_sources})


target_sources(table PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/localtable.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/table.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/table_plugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tablecolumn.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tableconcat.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tablefield.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tablegroup.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tablemodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tablemodelheader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tablemodelrowsinfo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tableroweach.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tableschema.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tabletolist.cpp"
)

target_include_directories(table
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/csvparser/single_include"
)
target_compile_definitions(table PRIVATE LV_table_PLUGIN)

target_link_libraries(table
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick )

target_link_libraries(table PRIVATE lvbase lvview)
