TEMPLATE = lib
TARGET   = lvbase
QT      += core qml quick
CONFIG  += qt c++17


## Win stack trace

CONFIG(debug, debug|release):ENABLE_STACK_TRACE = 1
win32:!isEmpty(ENABLE_STACK_TRACE){
    LIBS    += -ldbghelp
    DEFINES += USE_STACK_TRACE
}
unix:!isEmpty(ENABLE_STACK_TRACE_BFD){
    LIBS    += -lbfd -ldl
}

macx{
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

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

DEFINES += LV_BASE_LIB

include($$PWD/3rdparty/3rdparty.pri)
include($$PWD/include/lvbaseheaders.pri)
include($$PWD/src/lvbase.pri)
