PLUGIN_NAME = test
PLUGIN_PATH = test

TEMPLATE = lib
TARGET   = lvtest
CONFIG  += qt unversioned_libname skip_target_version_ext

linkLocalLibrary(lvbase,     lvbase)
linkLocalLibrary(lvview,     lvview)
linkLocalLibrary(lvelements, lvelements)

# Destination

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

# Source

unix:!macx{
    QMAKE_LFLAGS += \
        '-Wl,-rpath,\'\$$ORIGIN/../../link\''

    createlinkdir.commands += $${QMAKE_MKDIR_CMD} $$shell_path($${DEPLOY_PATH}/link)
    QMAKE_EXTRA_TARGETS    += createlinkdir
    POST_TARGETDEPS        += createlinkdir
}

include($$PWD/src/test.pri)

OTHER_FILES += \
    lv/*.lv \
    lv/live.module.json \
    lv/live.package.json


# --- Handling the lv deployment ---

PLUGIN_LV_DIR = $$_PRO_FILE_PWD_/lv

!exists($$PLUGIN_LV_DIR){
    warning(Expected folder $$PLUGIN_LV_DIR)
    qmlcopy.commands =
} else {
    qmlcopy.commands = $$deployDirCommand($$PLUGIN_LV_DIR, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH)
}

QMAKE_EXTRA_TARGETS += qmlcopy
POST_TARGETDEPS += qmlcopy
