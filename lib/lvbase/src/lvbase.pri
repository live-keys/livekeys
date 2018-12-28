INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/exception.h \
    $$PWD/libraryloadpath.h \
    $$PWD/lvbaseglobal.h \
    $$PWD/stacktrace.h \
    $$PWD/commandlineparser.h \
    $$PWD/lockedfileiosession.h \
    $$PWD/applicationcontext.h \
    $$PWD/mlnode.h \
    $$PWD/mlnodetojson.h \
    $$PWD/visuallog.h \
    $$PWD/indextuple.h \
    $$PWD/functionargs.h \
    $$PWD/lvglobal.h \
    $$PWD/package.h \
    $$PWD/plugin.h \
    $$PWD/version.h \
    $$PWD/packagegraph.h

SOURCES += \
    $$PWD/exception.cpp \
    $$PWD/commandlineparser.cpp \
    $$PWD/lockedfileiosession.cpp \
    $$PWD/applicationcontext.cpp \
    $$PWD/mlnode.cpp \
    $$PWD/mlnodetojson.cpp \
    $$PWD/libraryloadpath.cpp \
    $$PWD/visuallog.cpp \
    $$PWD/stacktrace.cpp \
    $$PWD/package.cpp \
    $$PWD/plugin.cpp \
    $$PWD/version.cpp \
    $$PWD/packagegraph.cpp

win32{
    SOURCES += $$PWD/stacktrace_win.cpp
    SOURCES += $$PWD/libraryloadpath_win.cpp
    SOURCES += $$PWD/applicationcontext_win.cpp
}

unix{
    !macx:SOURCES += $$PWD/applicationcontext_unix.cpp
    SOURCES += $$PWD/stacktrace_unix.cpp
    SOURCES += $$PWD/libraryloadpath_unix.cpp
}
macx:SOURCES += $$PWD/applicationcontext_mac.cpp

