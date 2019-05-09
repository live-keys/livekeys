
# If you need to understand the build process in detail with all its magic,
# look at the following files:
# .qmake.config
# project/functions.pri
#
# They are also included in this confiuration for completeness and easy access.

# --- Version Check ---

# Some linux distributions have qt version 4 already installed. Sometimes this
# can lead to running the wrong qmake version. Notify the user:

!qtVersionCheck(5, 6)::error( \
    This project requires at least Qt version 5.6. \
    Make sure you have Qt 5.6 installed and running the correct qmake. \
)

# --- Project structure ---

TEMPLATE = subdirs

SUBDIRS += \
    application \
    lib \
    plugins \
    tests

# --- Subdir configurations ---
application.subdir  = $$PWD/application
plugins.subdir      = $$PWD/plugins
tests.subdir        = $$PWD/tests

# --- Dependency configuration ---
application.depends = lib
plugins.depends     = lib
tests.depends       = lib

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

OTHER_FILES *= livecv.json
OTHER_FILES *= project/*.pri
OTHER_FILES *= doc/src/Doxyfile
OTHER_FILES *= doc/pages/*.md
