
add_library(timeline SHARED)

set_target_properties(timeline PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/timeline"
)

if(APPLE)
    set_target_properties(timeline PROPERTIES LINK_FLAGS
        "-Wl,-rpath,@executable_path/../Link -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@executable_path/../Frameworks/Live.framework/Libraries")
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(timeline PROPERTIES LINK_FLAGS
        "-Wl,-rpath,$$ORIGIN/../../link"
    )
endif()


add_output_files(timeline_qml        ${CMAKE_CURRENT_SOURCE_DIR}/qml        ${MODULE_DEPLOY_PATH}/timeline          timeline_qml_sources)
add_output_files(timeline_palettes   ${CMAKE_CURRENT_SOURCE_DIR}/palettes   ${MODULE_DEPLOY_PATH}/timeline/palettes timeline_palettes_sources)

target_sources(timeline PRIVATE ${timeline_qml})
target_sources(timeline PRIVATE ${timeline_palettes})


target_sources(timeline PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qmlgantt/lib/qrangeview.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qmlgantt/lib/qabstractrangemodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qmlgantt/lib/qganttmodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qmlgantt/lib/qganttmodelitem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/keyframe.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/keyframetrack.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/keyframevalue.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/timeline_plugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/timeline.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/timelineconfig.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/timelineheadermodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/segment.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/segmentmodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/timelineobjectproperty.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/timelineproperties.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/timelinesettings.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/track.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tracklistmodel.cpp"
)

target_include_directories(timeline
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qmlgantt/lib"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_definitions(timeline PRIVATE LV_TIMELINE_LIB Q_GANTT_LIB)

target_link_libraries(timeline
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick )

target_link_libraries(timeline PRIVATE lvbase lvview)
