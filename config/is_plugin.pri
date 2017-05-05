# This file contains the basic configuration for all plugins shipped with
# the application

!defined(PLUGIN_NAME, var): \
    error($$_FILE_: Tried to set up a plugin without defining PLUGIN_NAME first.)

isEmpty(PLUGIN_NAME): \
    error($$_FILE_: Tried to set up a plugin with an empty PLUGIN_NAME.)

# The PLUGIN_QML_DIR contains the path to the folder with all the qml files
# and related files

!defined(PLUGIN_QML_DIR, var): \
    error($$_FILE_: Tried to set up a plugin without defining PLUGIN_QML_DIR first.)

isEmpty(PLUGIN_QML_DIR): \
    error($$_FILE_: Tried to set up a plugin with an empty PLUGIN_QML_DIR.)

message(Configuring plugin $$PLUGIN_NAME)

TEMPLATE    = lib
QT          += qml quick
CONFIG      += qt plugin
TARGET      = $$PLUGIN_NAME
DESTDIR     = $$PATH_DEPLOY_PLUGINS/$$PLUGIN_NAME

# --- Handling the (ugly) QML deployment ---

QML_DEPLOY_FROM         = $$shell_path($$PLUGIN_QML_DIR)
QMLDIR_DEPLOY_FROM      = $$shell_path($$PLUGIN_QML_DIR/../qmldir)
PLUGIN_DEPLOY_TO        = $$shell_path($$PATH_DEPLOY_PLUGINS/$$PLUGIN_NAME)

!exists($$QMLDIR_DEPLOY_FROM){
    warning(Expected file $$QMLDIR_DEPLOY_FROM)
    qmldircopy.commands =
} else {
    qmldircopy.commands = \
    $$QMAKE_COPY $$QMLDIR_DEPLOY_FROM $$PLUGIN_DEPLOY_TO
}

first.depends = $(first) qmlcopy qmldircopy
export(first.depends)
export(qmlcopy.commands)
export(qmldircopy.commands)

QMAKE_EXTRA_TARGETS += first qmlcopy qmldircopy
