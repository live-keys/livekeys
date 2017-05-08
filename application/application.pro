TEMPLATE = app
TARGET   = livecv
QT      += qml quick

include($$getConfigFile(use_lcveditor.pri))
include($$getConfigFile(use_qmljsparser.pri))
include($$getConfigFile(use_plugin_live.pri))

DESTDIR = $$PATH_DEPLOY_APPLICATION

# Load library paths
# ------------------

unix{

    QMAKE_LFLAGS += \
        "-Wl,-rpath,\'\$$ORIGIN\'" \
        "-Wl,-rpath,\'\$$ORIGIN/plugins\'" \
        "-Wl,-rpath,\'\$$ORIGIN/plugins/live\'" \
        "-Wl,-rpath,\'\$$ORIGIN/plugins/lcvcore\'"

}

# Application
# -----------

include($$PWD/src/base/base.pri)
include($$PWD/src/main/main.pri)

RC_ICONS = $$PWD/icons/livecv.ico

RESOURCES += $$PWD/application.qrc

OTHER_FILES += \
    qml/*

PLUGIN_DEPLOY_FROM  = $$shell_path($$PATH_SOURCE_APPLICATION/qml)
PLUGIN_DEPLOY_TO    = $$shell_path($$PATH_DEPLOY_APPLICATION)

plugincopy.commands = $$QMAKE_COPY_DIR $$PLUGIN_DEPLOY_FROM $$PLUGIN_DEPLOY_TO

first.depends = $(first) plugincopy
export(first.depends)
export(plugincopy.commands)

QMAKE_EXTRA_TARGETS += first plugincopy

