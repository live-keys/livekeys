TEMPLATE = lib
TARGET   = lveditqmljs
QT      += core qml quick xml
CONFIG  += qt c++11

linkLocalLibrary(lvbase,   lvbase)
linkLocalLibrary(lvview,  lvview)
linkLocalLibrary(lveditor, lveditor)

DEFINES += LV_EDITQMLJS_LIB

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

#ENABLE_PLUGINTYPES = true

!isEmpty(ENABLE_PLUGINTYPES){
    message(Plugin Types Enabled: Livekeys will use private libraries)
    QT += core-private qml-private quick-private
    DEFINES += Q_PLUGINTYPES_ENABLED
}

include($$PWD/src/lveditqmljs.pri)
include($$PWD/3rdparty/3rdparty.pro)
include($$PWD/plugintypes/plugintypes.pri)
include($$PWD/include/lveditqmljsheaders.pri)
