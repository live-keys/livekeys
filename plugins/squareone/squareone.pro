PLUGIN_NAME = squareone
PLUGIN_PATH = squareone
uri = squareone

TEMPLATE = subdirs

# The PLUGIN_QML_DIR contains the path to the folder with all the qml files
# and related files
isEmpty(PLUGIN_QML_DIR): PLUGIN_QML_DIR = $$_PRO_FILE_PWD_/qml

# Deploy the tutorials

qmlcopy.commands = $$deployDirCommand($$PLUGIN_QML_DIR, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH)
tutorialcopy.commands = $$deployDirCommand($$PWD/tutorials, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_NAME/tutorials)
first.depends = $(first) qmlcopy tutorialcopy
export(first.depends)
export(tutorialcopy.commands)
export(qmlcopy.commands)
QMAKE_EXTRA_TARGETS += first qmlcopy tutorialcopy


OTHER_FILES += \
    qml/*.qml \
    qml/qmldir \
    qml/live.package.json \
    tutorials/workspace/*.* \
    tutorials/filters/*.*

DISTFILES += \
    tutorials/workspace/live.project.json

