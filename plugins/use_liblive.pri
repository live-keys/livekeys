# This file contains all common configuration features for plugins using the
# "live" library

# The PWD is the plugins directory

# Define a verbose option if not already done so
# Set this to "true" for makefile debugging and "false" for less clutter
!defined(VERBOSE, var): VERBOSE = false

# Setting all relevant paths
PATH_LIBIVE_INCLUDE     = $$PWD/../lib/live/include
PATH_LIBLIVE_LIB_DIR    = $$OUT_PWD/../../application

# Extra treatment for windows
win32:CONFIG(release, debug|release): \
    PATH_LIBLIVE_LIB_DIR = $$PATH_LIBLIVE_LIB_DIR/release
win32:CONFIG(debug, debug|release): \
    PATH_LIBLIVE_LIB_DIR = $$PATH_LIBLIVE_LIB_DIR/debug

# If verbose output is enabled, print the involved variables for debugging
if($$VERBOSE){
    message(PATH_LIBIVE_INCLUDE     = $$PATH_LIBIVE_INCLUDE)
    message(PATH_LIBLIVE_LIB_DIR    = $$PATH_LIBLIVE_LIB_DIR)
}

INCLUDEPATH += $$PATH_LIBIVE_INCLUDE
DEPENDPATH  += $$PATH_LIBIVE_INCLUDE
LIBS        += -L$$PATH_LIBLIVE_LIB_DIR -llive
