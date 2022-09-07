INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/bytebuffer.h \
    $$PWD/datetime.h \
    $$PWD/directory.h \
    $$PWD/exception.h \
    $$PWD/fileio.h \
    $$PWD/library.h \
    $$PWD/libraryloadpath.h \
    $$PWD/lvbaseglobal.h \
    $$PWD/module.h \
    $$PWD/modulecontext.h \
    $$PWD/palettecontainer.h \
    $$PWD/path.h \
    $$PWD/program.h \
    $$PWD/programholder.h \
    $$PWD/stacktrace.h \
    $$PWD/commandlineparser.h \
    $$PWD/applicationcontext.h \
    $$PWD/mlnode.h \
    $$PWD/mlnodetojson.h \
    $$PWD/typename.h \
    $$PWD/visuallog.h \
    $$PWD/indextuple.h \
    $$PWD/functionargs.h \
    $$PWD/lvglobal.h \
    $$PWD/package.h \
    $$PWD/version.h \
    $$PWD/packagegraph.h \
    $$PWD/packagecontext.h \
    $$PWD/utf8.h

SOURCES += \
    $$PWD/bytebuffer.cpp \
    $$PWD/datetime.cpp \
    $$PWD/directory.cpp \
    $$PWD/exception.cpp \
    $$PWD/commandlineparser.cpp \
    $$PWD/fileio.cpp \
    $$PWD/library.cpp \
    $$PWD/applicationcontext.cpp \
    $$PWD/mlnode.cpp \
    $$PWD/mlnodetojson.cpp \
    $$PWD/libraryloadpath.cpp \
    $$PWD/module.cpp \
    $$PWD/palettecontainer.cpp \
    $$PWD/path.cpp \
    $$PWD/program.cpp \
    $$PWD/typename.cpp \
    $$PWD/visuallog.cpp \
    $$PWD/stacktrace.cpp \
    $$PWD/package.cpp \
    $$PWD/version.cpp \
    $$PWD/packagegraph.cpp \
    $$PWD/utf8.cpp

win32{
    SOURCES += $$PWD/stacktrace_win.cpp
    SOURCES += $$PWD/libraryloadpath_win.cpp
    SOURCES += $$PWD/applicationcontext_win.cpp
    SOURCES += $$PWD/library_win.cpp
}

unix{
    !macx:SOURCES += $$PWD/applicationcontext_unix.cpp
    SOURCES += $$PWD/stacktrace_unix.cpp
    SOURCES += $$PWD/libraryloadpath_unix.cpp
    SOURCES += $$PWD/library_unix.cpp
}
macx:SOURCES += $$PWD/applicationcontext_mac.cpp

