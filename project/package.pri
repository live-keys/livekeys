
#//TODO Check if LIVECV_DEV_DIR, LIVECV_BIN_DIR have been set and exist

BUILD_PWD  = $$shadowed($$PROJECT_ROOT)
DEPLOY_PWD = $$LIVECV_BIN_DIR

include($$LIVECV_DEV_DIR/project/functions.pri)
