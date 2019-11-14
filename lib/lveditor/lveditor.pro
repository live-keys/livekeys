TEMPLATE = lib
TARGET   = lveditor
QT      += core qml quick quick-private
CONFIG  += qt

linkLocalLibrary(lvbase, lvbase)
linkLocalLibrary(lvview, lvview)
linkLocalLibrary(lvelements, lvelements)

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
}else:DESTDIR  = $$LIBRARY_DEPLOY_PATH

DEFINES += LV_EDITOR_LIB

include($$PWD/src/lveditor.pri)
include($$PWD/include/lveditorheaders.pri)
include($$PWD/3rdparty/3rdparty.pri)
