PLUGIN_NAME = nodeeditor
PLUGIN_PATH = workspace/nodeeditor

TEMPLATE = subdirs

# The PLUGIN_QML_DIR contains the path to the folder with all the qml files
# and related files
isEmpty(PLUGIN_QML_DIR): PLUGIN_QML_DIR = $$_PRO_FILE_PWD_/qml

# Deploy the samples

qmlcopy.commands = $$deployDirCommand($$PLUGIN_QML_DIR, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH)
samplescopy.commands = $$deployDirCommand($$PWD/samples, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH/samples)
first.depends = $(first) qmlcopy samplescopy
export(first.depends)
export(samplescopy.commands)
export(qmlcopy.commands)
QMAKE_EXTRA_TARGETS += first qmlcopy samplescopy

uri = workspace.nodeeditor

OTHER_FILES += \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes \
    samples/*.qml

DISTFILES += \
    qml/live.plugin.json \
    qml/qmldir
