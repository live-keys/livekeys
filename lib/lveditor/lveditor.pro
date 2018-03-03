TEMPLATE = lib
TARGET   = lveditor
QT      += core qml quick
CONFIG  += qt

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

DEFINES += LV_EDITOR_LIB

include($$PWD/src/lveditor.pri)
include($$PWD/include/lveditorheaders.pri)
