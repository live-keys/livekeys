PLUGIN_NAME = workspace
PLUGIN_PATH = workspace

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

qmlcopy.commands = $$deployDirCommand($$PWD/qml, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH)
first.depends = $(first) qmlcopy
export(first.depends)
export(qmlcopy.commands)
QMAKE_EXTRA_TARGETS += first qmlcopy

uri = workspace

TEMPLATE = subdirs

DISTFILES += \
    qml/live.package.json

SUBDIRS += \
    quickqanava \
    nodeeditor
