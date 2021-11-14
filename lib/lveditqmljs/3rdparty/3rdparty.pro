INCLUDEPATH += $$PWD

DEFINES += QTCREATOR_UTILS_STATIC_LIB # Utils as Static
DEFINES += LANGUAGEUTILS_BUILD_STATIC_LIB # Language Utils as Static
DEFINES += QML_BUILD_STATIC_LIB # QmlJS Parser as Static
#DEFINES += QT_QMLDEVTOOLS_LIB # QmlJS Static
DEFINES += QT_CREATOR

# Utils

HEADERS += \
    $$PWD/utils/utils_global.h \
    $$PWD/utils/qtcassert.h

SOURCES += \
    $$PWD/utils/qtcassert.cpp


# QmlJS

HEADERS += \
    $$PWD/qmljs/qmljs_global.h \
    $$PWD/qmljs/qmljsbind.h \
    $$PWD/qmljs/qmljsconstants.h \
    $$PWD/qmljs/qmljsdialect.h \
    $$PWD/qmljs/qmljsdocument.h \
    $$PWD/qmljs/qmljslineinfo.h \
    $$PWD/qmljs/qmljsscanner.h \
    $$PWD/qmljs/qmljsimportdependencies.h \
    $$PWD/qmljs/qmljsutils.h \
    $$PWD/qmljs/qmljsvalueowner.h \
    $$PWD/qmljs/qmljsinterpreter.h \
    $$PWD/qmljs/qmljstypedescriptionreader.h \
    $$PWD/qmljs/qmljscontext.h \
    $$PWD/qmljs/qmljsviewercontext.h \
    $$PWD/qmljs/qmljsqrcparser.h \
    $$PWD/qmljs/qmljsdescribevalue.h \
    $$PWD/qmljs/qmljscompletioncontextfinder.h

SOURCES += \
    $$PWD/qmljs/qmljsbind.cpp \
    $$PWD/qmljs/qmljsdialect.cpp \
    $$PWD/qmljs/qmljsdocument.cpp \
    $$PWD/qmljs/qmljslineinfo.cpp \
    $$PWD/qmljs/qmljsscanner.cpp \
    $$PWD/qmljs/qmljsimportdependencies.cpp \
    $$PWD/qmljs/qmljsutils.cpp \
    $$PWD/qmljs/qmljsvalueowner.cpp \
    $$PWD/qmljs/qmljsinterpreter.cpp \
    $$PWD/qmljs/qmljstypedescriptionreader.cpp \
    $$PWD/qmljs/qmljscontext.cpp \
    $$PWD/qmljs/qmljsviewercontext.cpp \
    $$PWD/qmljs/qmljsqrcparser.cpp \
    $$PWD/qmljs/qmljsdescribevalue.cpp \
    $$PWD/qmljs/qmljscompletioncontextfinder.cpp

# Language Utils

HEADERS += \
    $$PWD/languageutils/languageutils_global.h \
    $$PWD/languageutils/fakemetaobject.h \
    $$PWD/languageutils/componentversion.h

SOURCES += \
    $$PWD/languageutils/fakemetaobject.cpp \
    $$PWD/languageutils/componentversion.cpp

# QmlJS Parser

HEADERS += \
    $$PWD/qmljs/parser/qmldirparser_p.h \
    $$PWD/qmljs/parser/qmlerror.h \
    $$PWD/qmljs/parser/qqmljsast_p.h \
    $$PWD/qmljs/parser/qqmljsastfwd_p.h \
    $$PWD/qmljs/parser/qqmljsastvisitor_p.h \
    $$PWD/qmljs/parser/qqmljsengine_p.h \
    $$PWD/qmljs/parser/qqmljsglobal_p.h \
    $$PWD/qmljs/parser/qqmljsgrammar_p.h \
    $$PWD/qmljs/parser/qqmljskeywords_p.h \
    $$PWD/qmljs/parser/qqmljslexer_p.h \
    $$PWD/qmljs/parser/qqmljsmemorypool_p.h \
    $$PWD/qmljs/parser/qqmljsparser_p.h \
    $$PWD/qmljs/parser/qqmljsdiagnosticmessage_p.h \
    $$PWD/qmljs/parser/qqmljssourcelocation_p.h \
    $$PWD/qmljs/parser/qqmlapiversion_p.h

SOURCES += \
    $$PWD/qmljs/parser/qmldirparser.cpp \
    $$PWD/qmljs/parser/qmlerror.cpp \
    $$PWD/qmljs/parser/qqmljsast.cpp \
    $$PWD/qmljs/parser/qqmljsastvisitor.cpp \
    $$PWD/qmljs/parser/qqmljsengine_p.cpp \
    $$PWD/qmljs/parser/qqmljsgrammar.cpp \
    $$PWD/qmljs/parser/qqmljslexer.cpp \
    $$PWD/qmljs/parser/qqmljsparser.cpp








