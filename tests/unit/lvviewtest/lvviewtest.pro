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
    $$PWD/visuallogqttest.h \
    $$PWD/linecapturetest.h \
    $$PWD/memorytest.h

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/enginetest.cpp \
    $$PWD/mlnodetoqmltest.cpp \
    $$PWD/visuallogqttest.cpp \
    $$PWD/linecapturetest.cpp \
    $$PWD/memorytest.cpp


