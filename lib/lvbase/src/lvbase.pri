INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/exception.h \
    $$PWD/libraryloadpath.h \
    $$PWD/lvbaseglobal.h \
    $$PWD/stacktrace.h \
    $$PWD/commandlineparser.h \
    $$PWD/engine.h \
    $$PWD/settings.h \
    $$PWD/lockedfileiosession.h \
    $$PWD/incubationcontroller.h \
    $$PWD/errorhandler.h

SOURCES += \
    $$PWD/exception.cpp \
    $$PWD/libraryloadpath.cpp \
    $$PWD/stacktrace.cpp \
    $$PWD/commandlineparser.cpp \
    $$PWD/engine.cpp \
    $$PWD/settings.cpp \
    $$PWD/lockedfileiosession.cpp \
    $$PWD/incubationcontroller.cpp \
    $$PWD/errorhandler.cpp

win32{
    SOURCES += $$PWD/stacktrace_win.cpp
    SOURCES += $$PWD/libraryloadpath_win.cpp
}

unix{
    SOURCES += $$PWD/stacktrace_unix.cpp
    SOURCES += $$PWD/libraryloadpath_unix.cpp
}

