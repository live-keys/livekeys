set(PROJECT_SOURCES
    src/livekeys.cpp
    src/livekeysarguments.cpp
    src/main.cpp
    application.qrc
)

if (WIN32)
    enable_language("RC")
    list(APPEND PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/icons/livekeys.rc)

elseif(APPLE)
    set(livekeys_ICON ${CMAKE_CURRENT_SOURCE_DIR}/icons/livekeys.icns)
    set_source_files_properties(
        ${livekeys_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    list(APPEND PROJECT_SOURCES ${livekeys_ICON})
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(livekeys
        ${PROJECT_SOURCES}
    )
else()
    add_executable(livekeys
      ${PROJECT_SOURCES}
    )
endif()


if(APPLE)
    set_target_properties(livekeys PROPERTIES
        MACOSX_BUNDLE ON
        MACOSX_BUNDLE_BUNDLE_NAME "Livekeys"
        MACOSX_BUNDLE_BUNDLE_IDENTIFIER "com.livekeys.livekeys"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/os/ios/Info.plist"
    )

    # Configure rpaths

    set_target_properties(livekeys PROPERTIES LINK_FLAGS
        "-Wl,-rpath,@executable_path/../Link -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@executable_path/../Frameworks/Live.framework/Libraries")

elseif(UNIX)
    set_target_properties(livekeys PROPERTIES LINK_FLAGS
        "-Wl,-rpath,$$ORIGIN -Wl,-rpath,$$ORIGIN/link -Wl,-rpath,$$ORIGIN/lib"
    )

endif()

set_target_properties(livekeys PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_compile_definitions(livekeys
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(livekeys
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::WebEngine)

target_link_libraries(livekeys PRIVATE lvbase lvview lveditor)

if(SHOULD_BUILD_ELEMENTS_ENGINE)
    target_compile_definitions(livekeys PRIVATE BUILD_ELEMENTS BUILD_ELEMENTS_ENGINE)
    target_link_libraries(livekeys PRIVATE lvelementscompiler lvelementsengine)
endif()
