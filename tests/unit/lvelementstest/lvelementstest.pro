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
    $$PWD/mlnodetojstest.h \
    $$PWD/testpack.h \
    $$PWD/jstupletest.h \
    $$PWD/jsimportstest.h \
    $$PWD/jsinheritancetest.h \
    $$PWD/lvparsetest.h \
    $$PWD/metaobjecttypeinfotest.h \
    $$PWD/lvimportstest.h \
    $$PWD/lvelparseddocumenttest.h \
    $$PWD/lvellanguageinfoserializationtest.h

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
    $$PWD/mlnodetojstest.cpp \
    $$PWD/testpack.cpp \
    $$PWD/jstupletest.cpp \
    $$PWD/jsimportstest.cpp \
    $$PWD/jsinheritancetest.cpp \
    $$PWD/lvparsetest.cpp \
    $$PWD/metaobjecttypeinfotest.cpp \
    $$PWD/lvimportstest.cpp \
    $$PWD/lvelparseddocumenttest.cpp \
    $$PWD/lvellanguageinfoserializationtest.cpp

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

DISTFILES += \
    data/ImportTest01.lv \
    data/ImportTest02.lv \
    data/ImportTest03.lv \
    data/ImportTest04.lv \
    data/ImportTest05.lv \
    data/ImportTest10.lv \
    data/ImportTest07.lv \
    data/ImportTest08.lv \
    data/ImportTest09.lv \
    data/ImportTest06.lv \
    data/ImportTest11.lv \
    data/ParserTest1.lv \
    data/ParserTest1.lv.js \
    data/ParserTest11.lv \
    data/ParserTest14.lv \
    data/ParserTest14.lv.js \
    data/ParserTest2.lv \
    data/ParserTest2.lv.js \
    data/ParserTest3.lv \
    data/ParserTest3.lv.js \
    data/ParserTest4.lv \
    data/ParserTest4.lv.js \
    data/ParserTest5.lv \
    data/ParserTest5.lv.js \
    data/ParserTest6.lv \
    data/ParserTest6.lv.js \
    data/ParserTest7.lv \
    data/ParserTest7.lv.js \
    data/ParserTest8.lv \
    data/ParserTest8.lv.js \
    data/ParserTest9.lv \
    data/ParserTest9.lv.js