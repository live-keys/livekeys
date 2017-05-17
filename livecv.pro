
# If you need to understand the build process in detail with all its magic,
# look at the following files:
# .qmake.config
# config/config_functions.pri
# config/config_paths.pri
#
# They are also included in this confiuration for completeness and easy access.

# --- Version Check ---

# Some linux distributions have qt version 4 already installed. Sometimes this
# can lead to running the wrong qmake version. Notify the user:

!minQtVersion(5, 6)::error( \
    This project requires at least Qt version 5.6. \
    Make sure you have Qt 5.6 installed and running the correct qmake. \
)

# --- Project structure ---

TEMPLATE = subdirs

SUBDIRS += \
    application \
    editor \
    plugins

# --- Subdir configurations ---
application.subdir  = $$PWD/application
editor.subdir       = $$PWD/editor
plugins.subdir      = $$PWD/plugins

# --- Dependency configuration ---
application.depends = editor plugins # because we have a dependency to the live plugin here
plugins.depends     = editor


# Include the global configuration files since otherwise they would never show
# up in your project
OTHER_FILES += \
    .qmake.conf \
    $$CONFIG_DIR/*.pri

include($$PWD/config/config_functions.pri)
