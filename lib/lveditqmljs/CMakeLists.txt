
add_library(lveditqmljs SHARED)

set_target_properties(lveditqmljs PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_DEPLOY_PATH}"
)

add_subdirectory(3rdparty)
add_subdirectory(plugintypes)

target_sources(lveditqmljs PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/documentqmlchannels.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/documentqmlinfo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/documentqmlranges.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/languageqmlhandler.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlbindingchannel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlbindingchannelsdispatcher.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlbindingchannelsmodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlbindingpath.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmleditfragmentcontainer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlidvisitor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlcompletioncontext.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmllanguageinfo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlcompletioncontextfinder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/projectqmlscope.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmllibrarydependency.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/documentqmlvalueobjects.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/documentqmlvaluescanner.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmljssettings.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmljshighlighter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/projectqmlextension.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmljsbuiltintypes.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlmetainfo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlmetatypeinfo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlprojectmonitor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmladdcontainer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmleditfragment.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmldeclaration.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmldeclarationobject.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlimportsmodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmllanguagescanner.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmllanguagescanmonitor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlsyntax.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmltokenizer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlwatcher.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlcomponentsource.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlscopesnap.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlsuggestionmodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlusagegraphscanner.cpp"
)

target_compile_definitions(lveditqmljs PRIVATE LV_EDITQMLJS_LIB)
target_compile_features(lveditqmljs PRIVATE cxx_std_17)

target_include_directories(lveditqmljs
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(lveditqmljs
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::QuickPrivate Qt${QT_VERSION_MAJOR}::Xml )

#TODO: Add support for plugin types (include private)

target_link_libraries(lveditqmljs PRIVATE lvbase lvview lveditor)
