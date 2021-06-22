QT += quick svg

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


#QTQUICK_COMPILER_SKIPPED_RESOURCES += $$PWD/papergrapher.qrc
#QTQUICK_COMPILER_RETAINED_RESOURCES += $$PWD/papergrapher.qrc
RESOURCES += $$PWD/papergrapher.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


include($$PWD/paperqml/src/paper.pri)

HEADERS += $$PWD/samples/fileio.h

SOURCES += \
    $$PWD/samples/fileio.cpp \
    $$PWD/samples/main.cpp


DISTFILES += \
    notes.txt
