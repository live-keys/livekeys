
add_library(base SHARED)

set_target_properties(base PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/base"
    RUNTIME_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/base"
)

if(APPLE)
    set_target_properties(base PROPERTIES LINK_FLAGS
        "-Wl,-rpath,@executable_path/../Link -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@executable_path/../Frameworks/Live.framework/Libraries")
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(base PROPERTIES LINK_FLAGS
        "-Wl,-rpath,$$ORIGIN/../../link"
    )
endif()

add_output_files(base_qml      ${CMAKE_CURRENT_SOURCE_DIR}/qml      ${MODULE_DEPLOY_PATH}/base          base_qml_sources)
add_output_files(base_palettes ${CMAKE_CURRENT_SOURCE_DIR}/palettes ${MODULE_DEPLOY_PATH}/base/palettes base_palettes_sources)

target_sources(base PRIVATE ${base_qml_sources})
target_sources(base PRIVATE ${base_palettes_sources})


if(BUILD_PACKAGE_SAMPLES)
    add_output_files(base_sample_fileexplorer ${CMAKE_CURRENT_SOURCE_DIR}/samples/linkingexecutables ${MODULE_DEPLOY_PATH}/base/samples/linkingexecutables     base_sample_linkingexecutables_sources)
    add_output_files(base_sample_fileexplorer_res ${CMAKE_CURRENT_SOURCE_DIR}/samples/linkingexecutables/resources ${MODULE_DEPLOY_PATH}/base/samples/linkingexecutables/resources     base_sample_linkingexecutables_res_sources)
    target_sources(base PRIVATE ${base_sample_linkingexecutables_sources})
    target_sources(base PRIVATE ${base_sample_linkingexecutables_res_sources})
endif()

target_sources(base PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/base_plugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/environment.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlarrange.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlcollector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmldata.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlfunctionobjectcall.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlmain.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlmap.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlscriptargumentsparser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlexec.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlindexselector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlpropertylog.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlscript.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlsplit.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlstreamact.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlstreamoperations.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlstreamoperator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlstreamproviderscript.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlstreamvalue.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlstreamsink.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlstreamvalueawait.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlstreamvalueflow.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlthreadinfo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmltime.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlpromiseop.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlvalueflow.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlvalueflowgraph.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlvalueflowinput.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlvalueflowsink.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlworkerinterface.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlworkerpoolobject.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlstreamlog.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlact.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlcontainer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlstreamfilter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/workerthread.cpp"
)

target_include_directories(base
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_definitions(base PRIVATE LV_BASEQML_LIB)

target_link_libraries(base
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick )

target_link_libraries(base PRIVATE lvbase lvview lveditor lveditqmljs)
