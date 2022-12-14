
add_library(live SHARED)

set_target_properties(live PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/live"
)

if(APPLE)
    set_target_properties(live PROPERTIES LINK_FLAGS
        "-Wl,-rpath,@executable_path/../Link -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@executable_path/../Frameworks/Live.framework/Libraries")
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(live PROPERTIES LINK_FLAGS
        "-Wl,-rpath,$$ORIGIN/../../link"
    )
endif()


add_output_files(live_qml        ${CMAKE_CURRENT_SOURCE_DIR}/qml        ${MODULE_DEPLOY_PATH}/live          live_qml_sources)

target_sources(live PRIVATE ${live_qml_sources})


target_sources(live PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/eventrelay.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/live_plugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlcolor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/stringbasedloader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlcomponentmap.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlcomponentmapdata.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tcplineserver.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tcplinesocket.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tcplineconnection.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/worker.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlfork.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlforknode.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/remotecontainer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/remoteline.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/remotelineproperty.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/remotelineresponse.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/valuehistory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/licensesettings.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/loglistener.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/loglistenersocket.cpp"
)

target_include_directories(live
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_definitions(live PRIVATE Q_LIVE_LIB)

target_link_libraries(live
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick )

target_link_libraries(live PRIVATE lvbase lvview lveditor lveditqmljs base)
