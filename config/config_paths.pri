
initVar(VERBOSE, false)

# Cache the verbosity setting so it can be restored later
VERBOSE_CACHE = $$VERBOSE
VERBOSE = true # change the verbosity for local debugging output

# --- Preparing for build mode specific paths ---

# USE_BUILD_MODE_PATHS triggers the usage of specific build paths depending on whether
# you are building in debug or release mode
# This is enabled per default for windows builds and disabled per default for
# unix builds.
# You can override this default setting by either defining the variable
# beforehand or passing an own value to qmake

# Uncomment one of the following for a quick override
# USE_BUILD_MODE_PATHS = true
# USE_BUILD_MODE_PATHS = false

!defined(USE_BUILD_MODE_PATHS){
    win32:USE_BUILD_MODE_PATHS = true
    else: USE_BUILD_MODE_PATHS = false
}

# Generate the build path extension according to the build mode
!defined(BUILD_MODE_PATH_EXT){
    if($$USE_BUILD_MODE_PATHS){
              CONFIG(release, debug|release):   BUILD_MODE_PATH_EXT = /release
        else: CONFIG(debug,   debug|release):   BUILD_MODE_PATH_EXT = /debug
        else: error(Failed to determine build mode. Expected 'debug' or 'release'.)
    } else { # not using build mode paths
        BUILD_MODE_PATH_EXT = # Append nothing to the build path
    }
}

# --- Shared subpaths ---
# These subpaths are equal relative to the root of the source tree,
# build tree and deployment tree
# Subpaths do NOT include a leading or trailing slash

initVar(SUBPATH_APPLICATION, application)
initVar(SUBPATH_LIB, lib)
initVar(SUBPATH_LIB_LCVEDITOR, $$SUBPATH_LIB/lcveditor)
initVar(SUBPATH_LIB_QMLJSPARSER, $$SUBPATH_LIB/qmljsparser)

# --- Source tree paths ---
# NOTE: SOURCE_ROOT_PWD is defined in .qmake.conf to be globally viable
initVar(PATH_SOURCE_ROOT,               $$SOURCE_ROOT_PWD)
initVar(PATH_SOURCE_APPLICATION,        $$PATH_SOURCE_ROOT/$$SUBPATH_APPLICATION)
initVar(PATH_SOURCE_LIB,                $$PATH_SOURCE_ROOT/$$SUBPATH_LIB)
initVar(PATH_SOURCE_LIB_LCVEDITOR,      $$PATH_SOURCE_ROOT/$$SUBPATH_LIB_LCVEDITOR)
initVar(PATH_SOURCE_LIB_QMLJSPARSER,    $$PATH_SOURCE_ROOT/$$SUBPATH_LIB_QMLJSPARSER)
initVar(PATH_SOURCE_PLUGINS,            $$PATH_SOURCE_ROOT/plugins)

# --- Build tree paths ---
# NOTE: BUILD_ROOT_PWD is defined in .qmake.conf to be globally viable
initVar(PATH_BUILD_ROOT,        $$BUILD_ROOT_PWD$$BUILD_MODE_PATH_EXT) # c.f. Above
initVar(PATH_BUILD_APPLICATION, $$PATH_BUILD_ROOT/$$SUBPATH_APPLICATION)

initVar(PRINTED_PATHS_VERBOSE, false)
if($$VERBOSE : !PRINTED_PATHS_VERBOSE){
    message(--- Source tree setup ---)
    printVar(PATH_SOURCE_ROOT)
    printVar(PATH_SOURCE_APPLICATION)
    printVar(PATH_SOURCE_LIB)
    printVar(PATH_SOURCE_LIB_LCVEDITOR)
    printVar(PATH_SOURCE_LIB_QMLJSPARSER)
    printVar(PATH_SOURCE_PLUGINS)

    message(--- Build tree setup ---)
    printVar(PATH_BUILD_ROOT)
    printVar(PATH_BUILD_APPLICATION)

    PRINTED_PATHS_VERBOSE = false
}

#Restore the verbosity setting
VERBOSE = $$VERBOSE_CACHE


