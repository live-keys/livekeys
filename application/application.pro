TEMPLATE = app
TARGET   = livecv
QT      += qml quick

linkLocalLibrary(editor/lcveditor, lcveditor)
linkLocalLibrary(editor/qmljsparser, qmljsparser)

# Load library paths
# ------------------

unix{
    QMAKE_LFLAGS += \
        '-Wl,-rpath,\'\$$ORIGIN\'' \
        '-Wl,-rpath,\'\$$ORIGIN/link\''


    createlinkdir.commands += $${QMAKE_MKDIR_CMD} $$shell_path($${DEPLOY_PATH}/link)
    QMAKE_EXTRA_TARGETS    += createlinkdir
    POST_TARGETDEPS        += createlinkdir
}

DESTDIR = $$DEPLOY_PATH

# Application
# -----------

include($$PWD/src/base/base.pri)
include($$PWD/src/main/main.pri)

RC_ICONS = $$PWD/icons/livecv.ico

RESOURCES += $$PWD/application.qrc

OTHER_FILES += \
    $$PWD/qml/*.qml


