
target_sources(lveditqmljs PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/plugintypesfacade.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmlstreamwriter.cpp"
)

target_include_directories(lveditqmljs PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

if(BUILD_QML_PLUGINTYPES)
    message(EditQml: Plugin Types Enabled: editqml module will use private libraries)
    target_link_libraries(lveditqmljs Qt${QT_VERSION_MAJOR}::CorePrivate Qt${QT_VERSION_MAJOR}::QuickPrivate)
    target_compile_definitions(lveditqmljs PRIVATE Q_PLUGINTYPES_ENABLED)
endif()
