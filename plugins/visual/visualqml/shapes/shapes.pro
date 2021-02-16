PLUGIN_NAME = shapes
PLUGIN_PATH = visual/shapes
uri = visual.shapes

QT += svg

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

# Deploy the samples

qmlcopy.commands = $$deployDirCommand($$PLUGIN_QML_DIR, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH)
first.depends = $(first) qmlcopy
export(first.depends)
export(qmlcopy.commands)
QMAKE_EXTRA_TARGETS += first qmlcopy

include($$PWD/src/shapes.pri)

OTHER_FILES += \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes

