# This file contains all common configuration features for plugins using the
# "lcvcore" plugin as a base.

# Setting all relevant paths
PATH_LCVCORE_SOURCE     = $$PATH_SOURCE_PLUGINS_CORE/src
PATH_LCVCORE_INCLUDE    = $$PATH_SOURCE_PLUGINS_CORE/include

!exists($$PATH_LCVCORE_INCLUDE): \
    error($$_FILE_: Could not find $$PATH_LCVCORE_INCLUDE)

!exists($$PATH_LCVCORE_SOURCE): \
    error($$_FILE_: Could not find $$PATH_LCVCORE_SOURCE)


INCLUDEPATH += $$PATH_LCVCORE_INCLUDE
DEPENDPATH  += $$PATH_LCVCORE_SOURCE
LIBS        += -L$$PATH_DEPLOY_PLUGINS_CORE -llcvcore

# Since "core" is a plugin, it will be installed into the plugin subdirectory
# of the build
