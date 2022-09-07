TARGET   = lvbasetest
TEMPLATE = app
QT      += qml quick testlib
CONFIG  += console testcase c++17

linkLocalLibrary(lvbase,  lvbase)

HEADERS += \
    $$PWD/testrunner.h \
    $$PWD/commandlineparsertest.h \
    $$PWD/mlnodetest.h \
    $$PWD/mlnodetojsontest.h \
    $$PWD/filesystemtest.h \
    $$PWD/datetimetest.h \
    $$PWD/bytebuffertest.h \
    $$PWD/visuallogtest.h

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/commandlineparsertest.cpp \
    $$PWD/mlnodetest.cpp \
    $$PWD/mlnodetojsontest.cpp \
    $$PWD/filesystemtest.cpp \
    $$PWD/datetimetest.cpp \
    $$PWD/bytebuffertest.cpp \
    $$PWD/visuallogtest.cpp


