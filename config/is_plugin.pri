# This file contains the basic configuration for all plugins shipped with
# the application

!defined(PLUGIN_NAME, var): \
    error($$_FILE_: Tried to set up a plugin without defining PLUGIN_NAME first.)

isEmpty(PLUGIN_NAME): \
    error($$_FILE_: Tried to set up a plugin with an empty PLUGIN_NAME.)

!defined(PLUGIN_PATH, var): \
    error($$_FILE_: Tried to set up a plugin without defining PLUGIN_PATH first.)

isEmpty(PLUGIN_PATH): \
    error($$_FILE_: Tried to set up a plugin with an empty PLUGIN_PATH.)

!exists($$PLUGIN_PATH): \
    error($$_FILE_: Plugin path $$PLUGIN_PATH does not exist.)

# The PLUGIN_QML_DIR contains the path to the folder with all the qml files
# and related files
PLUGIN_QML_DIR = $$PLUGIN_PATH/qml

!defined(PLUGIN_QML_DIR, var): \
    error($$_FILE_: Tried to set up a plugin without defining PLUGIN_QML_DIR first.)

isEmpty(PLUGIN_QML_DIR): \
    error($$_FILE_: Tried to set up a plugin with an empty PLUGIN_QML_DIR.)

debug(Configuring plugin $$PLUGIN_NAME, 1)

TEMPLATE    = lib
QT          += qml quick
CONFIG      += qt plugin
TARGET      = $$PLUGIN_NAME
DESTDIR     = $$PATH_DEPLOY_PLUGINS/$$PLUGIN_NAME

# --- Handling the QML deployment ---

!exists($$PLUGIN_QML_DIR){
    warning(Expected folder $$PLUGIN_QML_DIR)
    qmlcopy.commands =
} else {
    qmlcopy.commands = $$deployDirCommand($$PLUGIN_QML_DIR, $$PATH_DEPLOY_PLUGINS/$$PLUGIN_NAME)
}

first.depends = $(first) qmlcopy

export(first.depends)
export(qmlcopy.commands)

QMAKE_EXTRA_TARGETS += first qmlcopy

# Deploying the qmldir files seperately is currently not required.

#QMLDIR_DEPLOY_FROM      = $$shell_path($$PLUGIN_QML_DIR/qmldir)
#!exists($$QMLDIR_DEPLOY_FROM){
#    warning(Expected file $$QMLDIR_DEPLOY_FROM)
#    qmldircopy.commands =
#} else {
#    qmldircopy.commands = \
#    $$QMAKE_COPY $$QMLDIR_DEPLOY_FROM $$PLUGIN_DEPLOY_TO
#}
# first.depends = $(first) qmldircopy
#export(qmldircopy.commands)
#QMAKE_EXTRA_TARGETS += first qmldircopy
