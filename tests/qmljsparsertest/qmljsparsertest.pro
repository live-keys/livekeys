TARGET   = unit_tests
TEMPLATE = app
QT      += qml quick testlib
CONFIG  += console testcase

INCLUDEPATH += $$PWD/../../editor/qmljsparser/src

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../editor/qmljsparser/release/ -lqmljsparser
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../editor/qmljsparser/debug/ -lqmljsparser
else:unix: LIBS += -L$$OUT_PWD/../../application/ -lqmljsparser

# Add files to test

HEADERS += \
    qtestrunner.h \
    qdocumentqmlparsertest.h

SOURCES += \
    main.cpp \
    qtestrunner.cpp \
    qdocumentqmlparsertest.cpp
