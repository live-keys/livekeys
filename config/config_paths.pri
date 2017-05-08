
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

initVar(SUBPATH_EDITOR, editor)
initVar(SUBPATH_EDITOR_LCVEDITOR, $$SUBPATH_EDITOR/lcveditor)
initVar(SUBPATH_EDITOR_QMLJSPARSER, $$SUBPATH_EDITOR/qmljsparser)

initVar(SUBPATH_PLUGINS, plugins)
initVar(SUBPATH_PLUGINS_CORE, $$SUBPATH_PLUGINS/lcvcore)
initVar(SUBPATH_PLUGINS_FEATURES2D, $$SUBPATH_PLUGINS/lcvfeatures2d)
initVar(SUBPATH_PLUGINS_IMGPROC, $$SUBPATH_PLUGINS/lcvimgproc)
initVar(SUBPATH_PLUGINS_LIVE, $$SUBPATH_PLUGINS/live)
initVar(SUBPATH_PLUGINS_PHOTO, $$SUBPATH_PLUGINS/lcvphoto)
initVar(SUBPATH_PLUGINS_VIDEO, $$SUBPATH_PLUGINS/lcvvideo)


# --- Source tree paths ---
# NOTE: SOURCE_ROOT_PWD is defined in .qmake.conf to be globally viable
initVar(PATH_SOURCE_ROOT,               $$SOURCE_ROOT_PWD)
initVar(PATH_SOURCE_APPLICATION,        $$PATH_SOURCE_ROOT/$$SUBPATH_APPLICATION)
initVar(PATH_SOURCE_EDITOR,             $$PATH_SOURCE_ROOT/$$SUBPATH_EDITOR)
initVar(PATH_SOURCE_EDITOR_LCVEDITOR,   $$PATH_SOURCE_ROOT/$$SUBPATH_EDITOR_LCVEDITOR)
initVar(PATH_SOURCE_EDITOR_QMLJSPARSER, $$PATH_SOURCE_ROOT/$$SUBPATH_EDITOR_QMLJSPARSER)
initVar(PATH_SOURCE_PLUGINS,            $$PATH_SOURCE_ROOT/$$SUBPATH_PLUGINS)
initVar(PATH_SOURCE_PLUGINS_CORE,       $$PATH_SOURCE_ROOT/$$SUBPATH_PLUGINS_CORE)
initVar(PATH_SOURCE_PLUGINS_FEATURES2D, $$PATH_SOURCE_ROOT/$$SUBPATH_PLUGINS_FEATURES2D)
initVar(PATH_SOURCE_PLUGINS_IMGPROC,    $$PATH_SOURCE_ROOT/$$SUBPATH_PLUGINS_IMGPROC)
initVar(PATH_SOURCE_PLUGINS_LIVE,       $$PATH_SOURCE_ROOT/$$SUBPATH_PLUGINS_LIVE)
initVar(PATH_SOURCE_PLUGINS_PHOTO,      $$PATH_SOURCE_ROOT/$$SUBPATH_PLUGINS_PHOTO)
initVar(PATH_SOURCE_PLUGINS_VIDEO,      $$PATH_SOURCE_ROOT/$$SUBPATH_PLUGINS_VIDEO)

# --- Deploy tree paths ---
# NOTE: DEPLOY_ROOT_PWD is defined in .qmake.conf to be globally viable
initVar(PATH_DEPLOY_ROOT,                $$DEPLOY_ROOT_PWD$$BUILD_MODE_PATH_EXT) # c.f. Above
initVar(PATH_DEPLOY_APPLICATION,         $$PATH_DEPLOY_ROOT)
initVar(PATH_DEPLOY_EDITOR,              $$PATH_DEPLOY_ROOT/$$SUBPATH_EDITOR)
initVar(PATH_DEPLOY_EDITOR_LCVEDITOR,    $$PATH_DEPLOY_ROOT/$$SUBPATH_EDITOR_LCVEDITOR)
initVar(PATH_DEPLOY_EDITOR_QMLJSPARSER,  $$PATH_DEPLOY_ROOT/$$SUBPATH_EDITOR_QMLJSPARSER)
initVar(PATH_DEPLOY_PLUGINS,             $$PATH_DEPLOY_ROOT/$$SUBPATH_PLUGINS)
initVar(PATH_DEPLOY_PLUGINS_CORE,        $$PATH_DEPLOY_ROOT/$$SUBPATH_PLUGINS_CORE)
initVar(PATH_DEPLOY_PLUGINS_FEATURES2D,  $$PATH_DEPLOY_ROOT/$$SUBPATH_PLUGINS_FEATURES2D)
initVar(PATH_DEPLOY_PLUGINS_IMGPROC,     $$PATH_DEPLOY_ROOT/$$SUBPATH_PLUGINS_IMGPROC)
initVar(PATH_DEPLOY_PLUGINS_LIVE,        $$PATH_DEPLOY_ROOT/$$SUBPATH_PLUGINS_LIVE)
initVar(PATH_DEPLOY_PLUGINS_PHOTO,       $$PATH_DEPLOY_ROOT/$$SUBPATH_PLUGINS_PHOTO)
initVar(PATH_DEPLOY_PLUGINS_VIDEO,       $$PATH_DEPLOY_ROOT/$$SUBPATH_PLUGINS_VIDEO)

# NOTE: It would be nice to have a plugin auto-discovery and setup mechanism...

