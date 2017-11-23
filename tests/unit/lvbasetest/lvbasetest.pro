TARGET   = lvbasetest
TEMPLATE = app
QT      += qml quick testlib
CONFIG  += console testcase

linkLocalLibrary(lvbase, lvbase)

include($$PWD/lvbasestubs/lvbasestubs.pri)

HEADERS += \
    $$PWD/testrunner.h \
    $$PWD/commandlineparsertest.h \
    $$PWD/enginetest.h \
    $$PWD/mlnodetest.h \
    $$PWD/mlnodetojsontest.h \
    $$PWD/mlnodetojstest.h \
    $$PWD/visuallogtest.h \
    filtertest.h

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/commandlineparsertest.cpp \
    $$PWD/enginetest.cpp \
    $$PWD/mlnodetest.cpp \
    $$PWD/mlnodetojsontest.cpp \
    $$PWD/mlnodetojstest.cpp \
    $$PWD/visuallogtest.cpp \
    filtertest.cpp


