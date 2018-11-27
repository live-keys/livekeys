INCLUDEPATH += $$PWD \
               $$PWD/../../lib/lveditor/src \
               $$PWD/../../lib/lveditor/include \
               $$PWD/../../lib/lveditor/3rdparty

QT      += qml quick quick-private qml-private

HEADERS += \
    $$PWD/livecv.h \
    $$PWD/livecvarguments.h \
    $$PWD/livecvscript.h \
    $$PWD/commands.h \
    $$PWD/environment.h

SOURCES += \
    $$PWD/livecv.cpp \
    $$PWD/livecvarguments.cpp \
    $$PWD/livecvscript.cpp \
    $$PWD/main.cpp \
    $$PWD/commands.cpp \
    $$PWD/environment.cpp
