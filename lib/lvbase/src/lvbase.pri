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
    $$PWD/errorhandler.h \
    $$PWD/plugincontext.h \
    $$PWD/mlnode.h \
    $$PWD/mlnodetojson.h \
    $$PWD/mlnodetojs.h \
    $$PWD/visuallognetworksender.h \
    $$PWD/visuallog.h \
    $$PWD/visuallogmodel.h \
    $$PWD/visuallogqt.h \
    $$PWD/visuallogjsobject.h \
    $$PWD/filter.h \
    $$PWD/shareddata.h \
    $$PWD/filterworker.h \
    $$PWD/filterworker_p.h \
    $$PWD/keymap.h \
    $$PWD/visuallogfilter.h \
    $$PWD/visuallogbasemodel.h \
    $$PWD/typeinfo.h

SOURCES += \
    $$PWD/exception.cpp \
    $$PWD/libraryloadpath.cpp \
    $$PWD/stacktrace.cpp \
    $$PWD/commandlineparser.cpp \
    $$PWD/engine.cpp \
    $$PWD/settings.cpp \
    $$PWD/lockedfileiosession.cpp \
    $$PWD/incubationcontroller.cpp \
    $$PWD/errorhandler.cpp \
    $$PWD/plugincontext.cpp \
    $$PWD/mlnode.cpp \
    $$PWD/mlnodetojson.cpp \
    $$PWD/mlnodetojs.cpp \
    $$PWD/visuallognetworksender.cpp \
    $$PWD/visuallog.cpp \
    $$PWD/visuallogmodel.cpp \
    $$PWD/visuallogjsobject.cpp \
    $$PWD/filter.cpp \
    $$PWD/shareddata.cpp \
    $$PWD/filterworker.cpp \
    $$PWD/keymap.cpp \
    $$PWD/visuallogfilter.cpp \
    $$PWD/visuallogbasemodel.cpp \
    $$PWD/typeinfo.cpp

win32{
    SOURCES += $$PWD/stacktrace_win.cpp
    SOURCES += $$PWD/libraryloadpath_win.cpp
}

unix{
    SOURCES += $$PWD/stacktrace_unix.cpp
    SOURCES += $$PWD/libraryloadpath_unix.cpp
}

