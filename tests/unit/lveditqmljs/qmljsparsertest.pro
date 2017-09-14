TARGET   = unit_tests
TEMPLATE = app
QT      += qml quick testlib
CONFIG  += console testcase

# Fix dependencies

INCLUDEPATH += $$PWD/../../editor/qmljsparser/src
INCLUDEPATH += $$PWD/../../editor/qmljsparser/plugintypes

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../editor/qmljsparser/release/ -lqmljsparser
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../editor/qmljsparser/debug/ -lqmljsparser
else:unix: LIBS += -L$$OUT_PWD/../../application -lqmljsparser

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

# Add files to test

include($$PWD/src/qmljsparsertest.pri)

OTHER_FILES += \
    $$PWD/data/mainwindow.in.qml \
    $$PWD/data/propertyexport.in.qml \
    $$PWD/data/scrollview.in.qml

DISTFILES += \
    data/customtype.in.qml
