TARGET   = lvbasetest
TEMPLATE = app
QT      += qml quick testlib
CONFIG  += console testcase

linkLocalLibrary(lib/lvbase, lvbase)

include($$PWD/lvbasestubs/lvbasestubs.pri)

HEADERS += \
    $$PWD/testrunner.h \
    $$PWD/commandlineparsertest.h \
    $$PWD/enginetest.h \
    $$PWD/mlnodetest.h

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/commandlineparsertest.cpp \
    $$PWD/enginetest.cpp \
    $$PWD/mlnodetest.cpp


