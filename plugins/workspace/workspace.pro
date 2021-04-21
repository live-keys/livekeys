PLUGIN_NAME = workspace
PLUGIN_PATH = workspace

# The PLUGIN_QML_DIR contains the path to the folder with all the qml files
# and related files
isEmpty(PLUGIN_QML_DIR): PLUGIN_QML_DIR = $$_PRO_FILE_PWD_/qml

qmlcopy.commands = $$deployDirCommand($$PWD/qml, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH)
first.depends = $(first) qmlcopy
export(first.depends)
export(qmlcopy.commands)
QMAKE_EXTRA_TARGETS += first qmlcopy

uri = workspace

TEMPLATE = subdirs

OTHER_FILES += \
    qml/* \
    qml/*.*

DISTFILES += \
    qml/live.package.json

SUBDIRS += \
    icons \
    quickqanava \
    nodeeditor
