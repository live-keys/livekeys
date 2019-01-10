TEMPLATE = lib
TARGET   = lvelements
QT      -= gui
CONFIG  += c++11 console
CONFIG  -= app_bundle

linkLocalLibrary(lvbase, lvbase)

win32:{
    DESTDIR    = $$DEPLOY_PATH/dev/$$TARGET/lib
    DLLDESTDIR = $$DEPLOY_PATH
}else:DESTDIR  = $$LIBRARY_DEPLOY_PATH


macx{
    QMAKE_LFLAGS += \
        '-Wl,-rpath,\'@executable_path/../Link\'' \
        '-Wl,-rpath,\'@executable_path/../Frameworks\''
    QMAKE_SONAME_PREFIX = @rpath/Live.framework/Libraries
}

DEFINES += LV_ELEMENTS_LIB


QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
QMAKE_MAC_SDK = macosx10.9

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


include($$PROJECT_ROOT/project/3rdparty/v8.pri)
include($$PWD/include/lvelementsheaders.pri)
include($$PWD/src/lvelements.pri)

deployV8()

OTHER_FILES += $$PWD/doc/*.dox

DISTFILES += \
    doc/page_v8.dox
