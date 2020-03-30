PLUGIN_NAME = squareone
PLUGIN_PATH = squareone

TEMPLATE = subdirs

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

uri = squareone

# Deploy the tutorials

tutorialcopy.commands = $$deployDirCommand($$PWD/tutorials, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_NAME/tutorials)
first.depends = $(first) tutorialcopy
export(first.depends)
export(tutorialcopy.commands)
QMAKE_EXTRA_TARGETS += first tutorialcopy


OTHER_FILES += \
    qml/*.qml \
    qml/qmldir \
    qml/live.package.json \
    tutorials/workspace/*.* \
    tutorials/filters/*.*

