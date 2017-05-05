
# Note: this setup requires you to use at least Qt 5
# (it is 2017, you should be doing so anyway)
#
# If you need to understand the build process in detail with all its magic,
# look at the following files:
# .qmake.config
# config/config_functions.pri
# config/config_paths.pri
#
# They are also included in this confiuration for completeness and easy access.

# --- Subdir configuration ---

TEMPLATE = subdirs

SUBDIRS += \
    application \
    editor \
    plugins

# Subdiŕ configurations
application.subdir  = $$PATH_SOURCE_APPLICATION
editor.subdir       = $$PATH_SOURCE_EDITOR
plugins.subdir      = $$PATH_SOURCE_PLUGINS

# Dependency configuration
application.depends = editor plugins # because we have a dependency to the live plugin here
plugins.depend      = editor



win32:DLLDESTDIR = $$PATH_DEPLOY_APPLICATION
CONFIG *= c++11 qml_debug

# Include the global configuration files since otherwise they would never show
# up in your project
OTHER_FILES += \
    .qmake.conf \
    $$CONFIG_DIR/*.pri
