# This file contains all common configuration features for using the
# "live" plugin

# Setting all relevant paths
LIBIVE_SOURCE_DIR     = $$PATH_SOURCE_PLUGINS_LIVE/src
LIBIVE_INCLUDE_DIR    = $$PATH_SOURCE_PLUGINS_LIVE/include

!exists($$LIBIVE_INCLUDE_DIR): \
    error($$_FILE_: Could not find $$LIBIVE_INCLUDE_DIR)

!exists($$LIBIVE_SOURCE_DIR): \
    error($$_FILE_: Could not find $$LIBIVE_SOURCE_DIR)

INCLUDEPATH += $$LIBIVE_INCLUDE_DIR
DEPENDPATH  += $$LIBIVE_SOURCE_DIR
LIBS        += -L$$PATH_DEPLOY_PLUGINS -llcvlive

# Since "live" is a plugin, it will be installed into the plugin subdirectory
# of the build
