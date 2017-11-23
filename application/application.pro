TEMPLATE = app
TARGET   = livecv
QT      += qml quick

linkLocalLibrary(lvbase,      lvbase)
linkLocalLibrary(lveditor,    lveditor)
linkLocalLibrary(lveditqmljs, lveditqmljs)

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

include($$PWD/src/main.pri)

RC_ICONS = $$PWD/icons/livecv.ico

RESOURCES += $$PWD/application.qrc

OTHER_FILES += \
    $$PWD/qml/*.qml

DISTFILES += \
    qml/MessageDialogMain.qml


