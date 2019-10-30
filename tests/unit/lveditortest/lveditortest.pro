TARGET   = lveditortest
TEMPLATE = app
QT      += qml quick testlib
CONFIG  += console testcase

linkLocalLibrary(lvbase, lvbase)
linkLocalLibrary(lvview, lvview)
linkLocalLibrary(lveditor,  lveditor)

HEADERS += \
    testrunner.h \
    lvlinecontroltest.h

SOURCES += \
    main.cpp \
    lvlinecontroltest.cpp
