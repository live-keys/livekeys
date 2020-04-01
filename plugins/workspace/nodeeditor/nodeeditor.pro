PLUGIN_NAME = nodeeditor
PLUGIN_PATH = workspace/nodeeditor

TEMPLATE = subdirs

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

# Deploy the samples

samplescopy.commands = $$deployDirCommand($$PWD/samples, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH/samples)
first.depends = $(first) samplescopy
export(first.depends)
export(samplescopy.commands)
QMAKE_EXTRA_TARGETS += first samplescopy

uri = workspace.nodeeditor

OTHER_FILES += \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes \
    samples/*.qml

DISTFILES += \
    qml/live.plugin.json \
    qml/qmldir

