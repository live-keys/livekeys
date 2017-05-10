TEMPLATE = app
TARGET   = livecv
QT      += qml quick

linkLocalLibrary(editor/lcveditor, lcveditor)
linkLocalLibrary(editor/qmljsparser, qmljsparser)
linkLocalPlugin(plugins/live, live)

printVar(LIBS)

# Load library paths
# ------------------

unix{
    QMAKE_LFLAGS += \
        '-Wl,-rpath,\'\$$ORIGIN\'' \
        '-Wl,-rpath,\'\$$ORIGIN/plugins/live\''
}

DESTDIR = $$buildModePath($$DEPLOY_PWD)

# Application
# -----------

include($$PWD/src/base/base.pri)
include($$PWD/src/main/main.pri)

RC_ICONS = $$PWD/icons/livecv.ico

RESOURCES += $$PWD/application.qrc

OTHER_FILES += \
    $$PWD/qml/*.qml


