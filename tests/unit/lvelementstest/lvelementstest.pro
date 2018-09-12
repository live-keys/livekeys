TARGET   = lvelementstest
TEMPLATE = app
QT      += qml quick testlib
CONFIG  += console testcase

linkLocalLibrary(lvbase,     lvbase)
linkLocalLibrary(lvelements, lvelements)

include($$PWD/lvelementsstubs/lvelementsstubs.pri)

HEADERS += \
    $$PWD/testrunner.h \
    $$PWD/jsobjecttest.h \
    $$PWD/metaobjectfunctiontest.h \
    $$PWD/eventtest.h \
    $$PWD/jseventtest.h \
    $$PWD/jspropertytest.h \
    $$PWD/jserrorhandlingtest.h \
    $$PWD/jsfunctiontest.h \
    $$PWD/jsmethodtest.h \
    $$PWD/jsmemorytest.h \
    $$PWD/jstypestest.h \
    $$PWD/jslisttest.h

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/jsobjecttest.cpp \
    $$PWD/metaobjectfunctiontest.cpp \
    $$PWD/eventtest.cpp \
    $$PWD/jseventtest.cpp \
    $$PWD/jspropertytest.cpp \
    $$PWD/jserrorhandlingtest.cpp \
    $$PWD/jsfunctiontest.cpp \
    $$PWD/jsmethodtest.cpp \
    $$PWD/jsmemorytest.cpp \
    $$PWD/jstypestest.cpp \
    $$PWD/jslisttest.cpp



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lvelements/release/ -llvelements
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lvelements/debug/ -llvelements
else:unix: LIBS += -L$$OUT_PWD/../lvelements/ -llvelements

INCLUDEPATH += $$PWD/../lvelements/include
DEPENDPATH += $$PWD/../lvelements/include
