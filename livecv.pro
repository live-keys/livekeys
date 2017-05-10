
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

if($$QT_MAJOR_VERSION<5):error( \
    This project requires at least Qt version 5. \
    Make sure you have Qt 5 installed and running the correct qmake. \
)

# --- Project structure ---

TEMPLATE = subdirs

SUBDIRS += \
    application \
    editor \
    plugins

# --- Subdir configurations ---
application.subdir  = application
editor.subdir       = editor
plugins.subdir      = plugins

# --- Dependency configuration ---
application.depends = editor plugins # because we have a dependency to the live plugin here
plugins.depends     = editor


# Include the global configuration files since otherwise they would never show
# up in your project
OTHER_FILES += \
    .qmake.conf

# Set CONFIG_DIR_IN_PROJECT_TREE in your build environment for quick access
# to qmake config files.
# This may require re-opening the project in the qtcreator since it has some
# issues with its file awareness
!defined(CONFIG_DIR_IN_PROJECT_TREE, var){
    OTHER_FILES *= $$CONFIG_DIR/*.pri
}

# NOTE: It is not required to explicitly
#include($$PWD/config/config_functions.pri)
#include($$PWD/config/config_paths.pri)
# .qmkae.conf already does that for you project-wide.
