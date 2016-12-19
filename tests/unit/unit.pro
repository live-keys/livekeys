TARGET   = unit_tests
TEMPLATE = app
QT      += qml quick testlib
CONFIG  += console testcase

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../editor/qmljsparser/release/ -lqmljsparser
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../editor/qmljsparser/debug/ -lqmljsparser
else:unix: LIBS += -L$$OUT_PWD/../../application/qmljsparser/ -lqmljsparser

INCLUDEPATH += $$PWD/../../editor/qmljsparser/src
DEPENDPATH += $$PWD/../../editor/qmljsparser/src


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../editor/lcveditor/release/ -llcveditor
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../editor/lcveditor/debug/ -llcveditor
else:unix: LIBS += -L$$OUT_PWD/../../application/lcveditor/ -llcveditor

INCLUDEPATH += $$PWD/../../editor/lcveditor/src
DEPENDPATH += $$PWD/../../editor/lcveditor/src

# Add files to test

include($$PWD/../../application/src/base/base.pri)

HEADERS += \
    qtestrunner.h \
    qlivecvcommandlineparsertest.h

SOURCES += \
    qtestrunner.cpp \
    main.cpp \
    qlivecvcommandlineparsertest.cpp


