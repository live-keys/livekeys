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

# The PLUGIN_QML_DIR contains the path to the folder with all the qml files
# and related files
isEmpty(PLUGIN_QML_DIR): PLUGIN_QML_DIR = $$_PRO_FILE_PWD_/qml

!defined(PLUGIN_QML_DIR, var): \
    error($$_FILE_: Tried to set up a plugin without defining PLUGIN_QML_DIR first.)

isEmpty(PLUGIN_QML_DIR): \
    error($$_FILE_: Tried to set up a plugin with an empty PLUGIN_QML_DIR.)

debug(Configuring plugin $$PLUGIN_NAME, 1)

TEMPLATE    = lib
QT          += qml quick
CONFIG      += qt plugin
TARGET      = $$PLUGIN_NAME

win32:{
    DESTDIR    = $$DEPLOY_PATH/dev/plugins/$$PLUGIN_PATH/lib
    isEmpty(DEPLOY_TO_LIVEKEYS)|equals(DEPLOY_TO_LIVEKEYS, false){
        !isEmpty(IS_PACKAGE):equals(IS_PACKAGE, true){
            DESTDIR = $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH/dev
        }
    }
    DLLDESTDIR = $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH
}else:DESTDIR = $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH

macx{
    QMAKE_LFLAGS += \
        '-Wl,-rpath,\'@executable_path/../Link\'' \
        '-Wl,-rpath,\'@executable_path/../Frameworks\''

    QMAKE_SONAME_PREFIX = @rpath
    QMAKE_LFLAGS += ' -install_name @rpath/lib$${TARGET}.dylib'

}

# --- Handling the QML deployment ---

!exists($$PLUGIN_QML_DIR){
    warning(Expected folder $$PLUGIN_QML_DIR)
    qmlcopy.commands =
} else {
    qmlcopy.commands = $$deployDirCommand($$PLUGIN_QML_DIR, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH)
}

QMAKE_EXTRA_TARGETS += qmlcopy
POST_TARGETDEPS += qmlcopy
