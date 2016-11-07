INCLUDEPATH += $$PWD

DEFINES += QTCREATOR_UTILS_STATIC_LIB # Utils as Static
DEFINES += LANGUAGEUTILS_BUILD_STATIC_LIB # Language Utils as Static
DEFINES += QML_BUILD_STATIC_LIB # QmlJS Parser as Static
DEFINES += QT_QMLDEVTOOLS_LIB # QmlJS Static
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
    $$PWD/qmljs/qmljsqrcparser.h

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
    $$PWD/qmljs/qmljsqrcparser.cpp

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
    $$PWD/qmljs/parser/qmljsast_p.h \
    $$PWD/qmljs/parser/qmljsastfwd_p.h \
    $$PWD/qmljs/parser/qmljsastvisitor_p.h \
    $$PWD/qmljs/parser/qmljsengine_p.h \
    $$PWD/qmljs/parser/qmljsglobal_p.h \
    $$PWD/qmljs/parser/qmljsgrammar_p.h \
    $$PWD/qmljs/parser/qmljskeywords_p.h \
    $$PWD/qmljs/parser/qmljslexer_p.h \
    $$PWD/qmljs/parser/qmljsmemorypool_p.h \
    $$PWD/qmljs/parser/qmljsparser_p.h

SOURCES += \
    $$PWD/qmljs/parser/qmldirparser.cpp \
    $$PWD/qmljs/parser/qmlerror.cpp \
    $$PWD/qmljs/parser/qmljsast.cpp \
    $$PWD/qmljs/parser/qmljsastvisitor.cpp \
    $$PWD/qmljs/parser/qmljsengine_p.cpp \
    $$PWD/qmljs/parser/qmljsgrammar.cpp \
    $$PWD/qmljs/parser/qmljslexer.cpp \
    $$PWD/qmljs/parser/qmljsparser.cpp








