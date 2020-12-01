TEMPLATE = lib
TARGET   = lvview
QT      += core qml quick quick-private
CONFIG  += qt

linkLocalLibrary(lvbase, lvbase)

macx{
    QMAKE_LFLAGS += \
        '-Wl,-rpath,\'@executable_path/../Link\'' \
        '-Wl,-rpath,\'@executable_path/../Frameworks\''
    QMAKE_SONAME_PREFIX = @rpath/Live.framework/Libraries
}

## Destination

win32:{
    DESTDIR    = $$DEPLOY_PATH/dev/$$TARGET/lib
    DLLDESTDIR = $$DEPLOY_PATH
}else:DESTDIR = $$LIBRARY_DEPLOY_PATH

DEFINES += LV_VIEW_LIB

include($$PWD/include/lvviewheaders.pri)
include($$PWD/src/lvview.pri)
