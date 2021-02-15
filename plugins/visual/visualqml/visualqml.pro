PLUGIN_NAME = visual
PLUGIN_PATH = visual

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

TEMPLATE = subdirs

qmlcopy.commands = $$deployDirCommand($$PLUGIN_QML_DIR, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH)
first.depends = $(first) qmlcopy
export(first.depends)
export(qmlcopy.commands)
QMAKE_EXTRA_TARGETS += first qmlcopy

OTHER_FILES += \
    $$PWD/qml/qmldir \
    $$PWD/qml/live.plugin.json \
    $$PWD/qml/live.package.json

SUBDIRS += \
    input


