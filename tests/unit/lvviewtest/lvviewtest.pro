TARGET   = lvbasetest
TEMPLATE = app
QT      += qml quick testlib
CONFIG  += console testcase

linkLocalLibrary(lvbase, lvbase)
linkLocalLibrary(lvview, lvview)

include($$PWD/lvviewstubs/lvviewstubs.pri)

HEADERS += \
    $$PWD/testrunner.h \
    $$PWD/enginetest.h \
    $$PWD/mlnodetoqmltest.h \
    $$PWD/visuallogtest.h \
    $$PWD/grouptest.h \
    $$PWD/linecapturetest.h

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/enginetest.cpp \
    $$PWD/mlnodetoqmltest.cpp \
    $$PWD/visuallogtest.cpp \
    $$PWD/grouptest.cpp \
    $$PWD/linecapturetest.cpp


