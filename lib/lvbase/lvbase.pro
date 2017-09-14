TEMPLATE = lib
TARGET   = lvbase
QT      += core qml quick
CONFIG  += qt


## Win stack trace

CONFIG(debug, debug|release):ENABLE_STACK_TRACE = 1
win32:!isEmpty(ENABLE_STACK_TRACE){
    LIBS    += -ldbghelp
    DEFINES += USE_STACK_TRACE
}
unix:!isEmpty(ENABLE_STACK_TRACE_BFD){
    LIBS    += -lbfd -ldl
}

## Destination

win32:{
    DESTDIR    = $$DEPLOY_PATH/dev/lib
    DLLDESTDIR = $$DEPLOY_PATH
}else:DESTDIR = $$DEPLOY_PATH

DEFINES += LVBASE_LIB

include($$PWD/3rdparty/3rdparty.pri)
include($$PWD/include/lvbaseheaders.pri)
include($$PWD/src/lvbase.pri)
