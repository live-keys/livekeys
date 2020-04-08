PLUGIN_NAME = timeline
PLUGIN_PATH = timeline

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

DEFINES += LV_TIMELINE_LIB

uri = timeline

linkLocalLibrary(lvbase, lvbase)
linkLocalLibrary(lvview, lvview)

# Source

unix:!macx{
    QMAKE_LFLAGS += \
	'-Wl,-rpath,\'\$$ORIGIN/../../link\''

    createlinkdir.commands += $${QMAKE_MKDIR_CMD} $$shell_path($${DEPLOY_PATH}/link)
    QMAKE_EXTRA_TARGETS    += createlinkdir
    POST_TARGETDEPS        += createlinkdir
}

include($$PWD/src/timeline.pri)
include($$PWD/include/timelineheaders.pri)
include($$PWD/3rdparty/qmlgantt/lib/qmlgantt.pri)

OTHER_FILES += \
    $$PWD/qml/*.qml \
    $$PWD/qml/qmldir \
    $$PWD/qml/live.package.json \
    $$PWD/qml/live.plugin.json

DISTFILES += \
    qml/SegmentView.qml
