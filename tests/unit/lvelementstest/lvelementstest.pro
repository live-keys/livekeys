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
    $$PWD/jslisttest.h \
    $$PWD/lvcompiletest.h \
    mlnodetojstest.h \
    testpack.h

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
    $$PWD/jslisttest.cpp \
    $$PWD/lvcompiletest.cpp \
    mlnodetojstest.cpp \
    testpack.cpp

OTHER_FILES += $$PWD/data/*.*


# Copy test data

TEST_DATA_DEPLOY_FROM = $$PWD/data

win32:CONFIG(debug, debug|release): TEST_DATA_DEPLOY_TO = $$OUT_PWD/debug/data
else:win32:CONFIG(release, debug|release): TEST_DATA_DEPLOY_TO = $$OUT_PWD/release/data
else:unix: TEST_DATA_DEPLOY_TO = $$OUT_PWD

win32:TEST_DATA_DEPLOY_TO ~= s,/,\\,g
win32:TEST_DATA_DEPLOY_FROM ~= s,/,\\,g

testdatacopy.commands = $(COPY_DIR) \"$$TEST_DATA_DEPLOY_FROM\" \"$$TEST_DATA_DEPLOY_TO\"
first.depends = $(first) testdatacopy
export(first.depends)
export(testdatacopy.commands)

QMAKE_EXTRA_TARGETS += first testdatacopy
