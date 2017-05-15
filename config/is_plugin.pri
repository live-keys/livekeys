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
PLUGIN_QMLDIR_FILE = $$PLUGIN_PATH/qmldir

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

# Check if the qmldir file is placed in the plugin dir seperately and deploy it as such
# otherwise it might already be deployed with the qml directory itself
!exists($$PLUGIN_QMLDIR_FILE){
    warning(No file $$PLUGIN_QMLDIR_FILE)
    qmldircopy.commands =
} else {
    qmldircopy.commands = $$deployFileCommand($$PLUGIN_QMLDIR_FILE, $$PATH_DEPLOY_PLUGINS/$$PLUGIN_NAME)
}

QMAKE_EXTRA_TARGETS += qmlcopy qmldircopy
POST_TARGETDEPS += qmlcopy qmldircopy
