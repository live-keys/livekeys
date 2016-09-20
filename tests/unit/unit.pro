TARGET   = unit_tests
TEMPLATE = app
QT      += qml quick testlib
CONFIG  += console testcase

# Add files to test

include($$PWD/../../application/src/base/base.pri)

HEADERS += \
    qtestrunner.h \
    qlivecvcommandlineparsertest.h

SOURCES += \
    qtestrunner.cpp \
    main.cpp \
    qlivecvcommandlineparsertest.cpp
