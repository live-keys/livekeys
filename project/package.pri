
# Check if LIVECV_BIN_PATH and LIVECV_DEV_PATH have been set. Only LIVECV_DEV_PATH has to
# exists, since LIVECV_BIN_PATH can be build together with our project.

isEmpty(LIVECV_BIN_PATH): \
    error($$_FILE_: You need to set LIVECV_BIN_PATH before including package.pri file.)
isEmpty(LIVECV_DEV_PATH): \
    error($$_FILE_: You need to set LIVECV_DEV_PATH before including package.pri file.)
!exists($$LIVECV_DEV_PATH): \
    error($$_FILE_: Dev path $$LIVECV_DEV_PATH does not exist.)

# Setup Paths

BUILD_PATH  = $$shadowed($$PROJECT_ROOT)
DEPLOY_PATH = $$BUILD_PATH/bin
PLUGIN_DEPLOY_PATH = $$DEPLOY_PATH
IS_PACKAGE  = true

!isEmpty(DEPLOY_TO_LIVECV):equals(DEPLOY_TO_LIVECV, true){
    DEPLOY_PATH             = $$LIVECV_BIN_PATH
    macx:PLUGIN_DEPLOY_PATH = $$DEPLOY_PATH/PlugIns
    else:PLUGIN_DEPLOY_PATH = $$DEPLOY_PATH/plugins
}

macx:LIBRARY_DEPLOY_PATH = $$DEPLOY_PATH/Frameworks/Live.framework
else:LIBRARY_DEPLOY_PATH = $$DEPLOY_PATH

macx:QMAKE_SONAME_PREFIX = @rpath

include($$PWD/functions.pri)
