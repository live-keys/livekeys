TARGET   = lvbasetest
TEMPLATE = app
QT      += qml quick testlib
CONFIG  += console testcase

linkLocalLibrary(lvbase,  lvbase)

HEADERS += \
    $$PWD/testrunner.h \
    $$PWD/commandlineparsertest.h \
    $$PWD/mlnodetest.h \
    $$PWD/mlnodetojsontest.h

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/commandlineparsertest.cpp \
    $$PWD/mlnodetest.cpp \
    $$PWD/mlnodetojsontest.cpp


