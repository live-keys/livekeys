TEMPLATE = app
TARGET   = livecv
QT      += qml quick

linkLocalLibrary(lvbase,      lvbase)
linkLocalLibrary(lvview,     lvview)
linkLocalLibrary(lveditor,    lveditor)
linkLocalLibrary(lveditqmljs, lveditqmljs)

# Load library paths
# ------------------

unix:!macx{
    QMAKE_LFLAGS += \
        '-Wl,-rpath,\'\$$ORIGIN\'' \
        '-Wl,-rpath,\'\$$ORIGIN/link\'' \
	'-Wl,-rpath,\'\$$ORIGIN/lib\''


    createlinkdir.commands += $${QMAKE_MKDIR_CMD} $$shell_path($${DEPLOY_PATH}/link)
    QMAKE_EXTRA_TARGETS    += createlinkdir
    POST_TARGETDEPS        += createlinkdir
}

macx{
    QMAKE_LFLAGS += \
        '-Wl,-rpath,\'@executable_path/../Link\'' \
        '-Wl,-rpath,\'@executable_path/../Frameworks\''

    createlinkdir.commands += $${QMAKE_MKDIR_CMD} $$shell_path($${DEPLOY_PATH}/Link)
    QMAKE_EXTRA_TARGETS    += createlinkdir
    POST_TARGETDEPS        += createlinkdir
}

DESTDIR = $$BUILD_PATH/bin

# Application
# -----------

include($$PWD/src/main.pri)

RC_ICONS = $$PWD/icons/livecv.ico
macx:ICON = $$PWD/icons/livecv.icns

RESOURCES += $$PWD/application.qrc

OTHER_FILES += \
    $$PWD/qml/*.qml

DISTFILES += \
    qml/MessageDialogMain.qml \
    qml/LogView.qml \
    qml/CommandsMenu.qml \
    qml/editorlayer.qml \
    qml/PaneSplitView.qml \
    qml/LiveKeysTheme.qml \
    qml/RunnablesMenu.qml


