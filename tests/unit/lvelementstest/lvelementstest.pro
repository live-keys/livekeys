TARGET   = lvelementstest
TEMPLATE = app
QT      += qml quick testlib
CONFIG  += console testcase

linkLocalLibrary(lvbase,     lvbase)
linkLocalLibrary(lvelements, lvelements)

include($$PROJECT_ROOT/project/3rdparty/v8.pri)
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
    $$PWD/jsinheritancetest.h \
    $$PWD/lvparsetest.h \
    $$PWD/metaobjecttypeinfotest.h \
    $$PWD/lvimportstest.h \
    $$PWD/lvelparseddocumenttest.h \
    $$PWD/lvellanguageinfoserializationtest.h \
    lvparseerrortest.h

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
    $$PWD/jsinheritancetest.cpp \
    $$PWD/lvparsetest.cpp \
    $$PWD/metaobjecttypeinfotest.cpp \
    $$PWD/lvimportstest.cpp \
    $$PWD/lvelparseddocumenttest.cpp \
    $$PWD/lvellanguageinfoserializationtest.cpp \
    lvparseerrortest.cpp

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
    data/ImportTest01.lvep \
    data/ImportTest02.lvep \
    data/ImportTest03.lvep \
    data/ImportTest04.lvep \
    data/ImportTest05.lvep \
    data/ImportTest06.lvep \
    data/ImportTest07.lvep \
    data/ImportTest08.lvep \
    data/ImportTest09.lvep \
    data/ImportTest10.lvep \
    data/ImportTest11.lvep \
    data/ImportTest12.lvep \
    data/ImportTest13.lvep \
    data/ParserErrorTest01.lv \
    data/ParserTest011.lv \
    data/ParserTest1.lv.js \
    data/ParserTest11.lv \
    data/ParserTest14.lv \
    data/ParserTest14.lv.js \
    data/ParserTest2.lv \
    data/ParserTest2.lv.js \
    data/ParserTest27.lv \
    data/ParserTest27.lv.js \
    data/ParserTest28.lv \
    data/ParserTest28.lv.js \
    data/ParserTest29.lv \
    data/ParserTest29.lv.js \
    data/ParserTest3.lv \
    data/ParserTest3.lv.js \
    data/ParserTest30.lv \
    data/ParserTest30.lv.js \
    data/ParserTest31.lv \
    data/ParserTest31.lv.js \
    data/ParserTest32.lv \
    data/ParserTest32.lv.js \
    data/ParserTest33.lv \
    data/ParserTest33.lv.js \
    data/ParserTest34.lv \
    data/ParserTest34.lv.js \
    data/ParserTest35.lv \
    data/ParserTest35.lv.js \
    data/ParserTest36.lv \
    data/ParserTest36.lv.js \
    data/ParserTest37.lv \
    data/ParserTest37.lv.js \
    data/ParserTest38.lv \
    data/ParserTest38.lv \
    data/ParserTest38.lv.js \
    data/ParserTest38.lv.js \
    data/ParserTest39.lv \
    data/ParserTest39.lv.js \
    data/ParserTest4.lv \
    data/ParserTest4.lv.js \
    data/ParserTest40.lv \
    data/ParserTest40.lv.js \
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
