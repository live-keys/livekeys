# This file contains all common configuration features for plugins using the
# "lcvcore" plugin as a base.

# The PWD is the plugins directory

# Define a verbose option if not already done so
# Set this to "true" for makefile debugging and "false" for less clutter
!defined(VERBOSE, var): VERBOSE = false

# Setting all relevant paths
PATH_LCVCORE_INCLUDE = $$PWD/lcvcore/include
PATH_LCVCORE_LIB_DIR = $$OUT_PWD/../../application/plugins

# Extra treatment for windows
win32:CONFIG(release, debug|release): \
    PATH_LCVCORE_LIB_DIR = $$PATH_LCVCORE_LIB_DIR/release
win32:CONFIG(debug, debug|release): \
    PATH_LCVCORE_LIB_DIR = $$PATH_LCVCORE_LIB_DIR/debug

# If verbose output is enabled, print the involved variables for debugging
if($$VERBOSE){
    message(PATH_LCVCORE_INCLUDE = $$PATH_LCVCORE_INCLUDE)
    message(PATH_LCVCORE_LIB_DIR = $$PATH_LCVCORE_LIB_DIR)
}

INCLUDEPATH += $$PATH_LCVCORE_INCLUDE
DEPENDPATH  += $$PATH_LCVCORE_INCLUDE
LIBS        += -L$$PATH_LCVCORE_LIB_DIR -llcvcore
