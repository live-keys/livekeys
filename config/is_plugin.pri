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

PLUGIN_DEPLOY_FROM  = $$shell_path($$PLUGIN_QML_DIR)
PLUGIN_DEPLOY_TO    = $$shell_path($$PATH_DEPLOY_PLUGINS/$$PLUGIN_NAME)

plugincopy.commands = $$QMAKE_COPY_DIR $$PLUGIN_DEPLOY_FROM $$PLUGIN_DEPLOY_TO

first.depends = $(first) plugincopy
export(first.depends)
export(plugincopy.commands)

QMAKE_EXTRA_TARGETS += first plugincopy
