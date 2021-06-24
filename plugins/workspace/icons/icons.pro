PLUGIN_NAME = icons
PLUGIN_PATH = workspace/icons
uri = workspace.icons

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))
TEMPLATE = aux

# Deploy the samples

qmlcopy.commands = $$deployDirCommand($$PLUGIN_QML_DIR, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH)
first.depends = $(first) qmlcopy
export(first.depends)
export(qmlcopy.commands)
QMAKE_EXTRA_TARGETS += first qmlcopy

OTHER_FILES += \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes

DISTFILES += \
    qml/BrushIcon.qml \
    qml/EventIcon.qml \
    qml/FunctionIcon.qml \
    qml/GradientIcon.qml

