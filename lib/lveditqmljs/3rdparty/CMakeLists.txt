
target_compile_definitions(lveditqmljs PRIVATE
    QTCREATOR_UTILS_STATIC_LIB
    LANGUAGEUTILS_BUILD_STATIC_LIB
    QML_BUILD_STATIC_LIB
    QT_CREATOR)

target_include_directories(lveditqmljs PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# Utils

target_sources(lveditqmljs PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/qtcassert.cpp"
)

# QmlJS

target_sources(lveditqmljs PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/qmljsbind.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/qmljsdialect.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/qmljsdocument.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/qmljslineinfo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/qmljsscanner.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/qmljsimportdependencies.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/qmljsutils.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/qmljsvalueowner.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/qmljsinterpreter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/qmljstypedescriptionreader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/qmljscontext.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/qmljsviewercontext.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/qmljsqrcparser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/qmljsdescribevalue.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/qmljscompletioncontextfinder.cpp"
)

# Language Utils


target_sources(lveditqmljs PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/languageutils/fakemetaobject.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/languageutils/componentversion.cpp"
)

# QmlJS Parser

target_sources(lveditqmljs PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/parser/qmldirparser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/parser/qmlerror.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/parser/qqmljsast.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/parser/qqmljsastvisitor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/parser/qqmljsengine_p.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/parser/qqmljsgrammar.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/parser/qqmljslexer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qmljs/parser/qqmljsparser.cpp"
)
