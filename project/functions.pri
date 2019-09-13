
# This file defines utility functions for use in the project build


# A utility function to abbreviate the hassle of checking if a variable is
# initialized and setting it to an initial value if not.
# Note that an empty variable is already defined and thus will not be altered.
#
# First argument is the variable name ,
# Second one the initial value
#
# Example:
#
# clear(FOO)
# initVar(FOO, bar)
# message($$FOO) # yields "bar", since FOO was undefined
# initVar(FOO, baz)
# message($$FOO) # still yields "bar" since FOO was already defined

defineTest(initVar){
    !defined($$1, var) {
        $$1 = $$2
        export($$1)
    } else {
        if($$VERBOSE):message($$1 already defined)
    }
}

# Prints the name and content of a variable for debugging
#
# First argument is a variable
#
# Example:
#
# FOO = bar
# printVar(FOO) # prints "FOO = bar"

defineTest(printVar){
    message($$1 = $$eval($$1))
}

defineTest(includeRequired){
    if($$VERBOSE): message(Checking for $$1)

    !exists($$file){
        error(Missing required file $$1)
    } else{
        include($$1)
        if($$VERBOSE): message(Included $$1)
    }
}

# Links a local library to the current project
#
# Args: (path, name, [include_dir])
#  * path: relative path to the library from root
#  * name: name of the library
#  * include_dir: include dir path(defaults to library path in source tree + '/include')

defineTest(linkLocalLibrary){

    win32:LIB_PATH = $$DEPLOY_PATH/dev/$$1/lib
    else:LIB_PATH = $$LIBRARY_DEPLOY_PATH

    LIB_NAME = $$2
    LIB_INCLUDE_PATH = $$PROJECT_ROOT/lib/$$1/include
    !isEmpty($$3):LIB_INCLUDE_PATH=$$3

    # use *= instead of += to prevent duplications of link path cofigurations
    LIBS *= -L$$LIB_PATH
    LIBS *= -l$$LIB_NAME
    INCLUDEPATH += $$LIB_INCLUDE_PATH
    DEPENDPATH  += $$LIB_INCLUDE_PATH
    export(LIBS)
    export(INCLUDEPATH)
    export(DEPENDPATH)

    debug(Linking: $$LIB_PATH -$$LIB_NAME with include path: $$LIB_INCLUDE_PATH, 1)
}

# Links a livekeys library to the current project
#
# Args: (path, name, [include_dir])
#  * path: relative path to the library from root
#  * name: name of the library
#  * include_dir: include dir path(defaults to library path in source tree + '/include')

defineTest(linkLibrary){

    win32:LIB_PATH = $$DEPLOY_PATH/dev/$$1/lib
    else:LIB_PATH = $$LIBRARY_DEPLOY_PATH

    LIB_NAME = $$2
    LIB_INCLUDE_PATH = $$PROJECT_ROOT/lib/$$1/include
    !isEmpty(LIVEKEYS_DEV_PATH):LIB_INCLUDE_PATH = $$LIVEKEYS_DEV_PATH/lib/$$1/include
    !isEmpty($$3):LIB_INCLUDE_PATH=$$3

    # use *= instead of += to prevent duplications of link path cofigurations
    LIBS *= -L$$LIB_PATH
    LIBS *= -l$$LIB_NAME
    INCLUDEPATH += $$LIB_INCLUDE_PATH
    DEPENDPATH  += $$LIB_INCLUDE_PATH
    export(LIBS)
    export(INCLUDEPATH)
    export(DEPENDPATH)

    debug(Linking: $$LIB_PATH -$$LIB_NAME with include path: $$LIB_INCLUDE_PATH, 1)
}

# Links a local plugin to the current project
#
# Args: (path, name, [include_dir])
#  * path: path to the plugin from 'plugins'
#  * name: name of the library
#  * include_dir: include dir path(defaults to library path in source tree + '/src')
#
defineTest(linkLocalPlugin){
    win32:LIB_PATH = $$DEPLOY_PATH/dev/plugins/$$1/lib
    else:LIB_PATH = $$PLUGIN_DEPLOY_PATH/$$1

    LIB_NAME = $$2
    LIB_INCLUDE_PATH = $$PROJECT_ROOT/plugins/$$1/include
    !isEmpty($$3):LIB_INCLUDE_PATH=$$3

    # use *= instead of += to prevent duplications of link path cofigurations
    LIBS *= -L$$LIB_PATH
    LIBS *= -l$$LIB_NAME
    INCLUDEPATH += $$LIB_INCLUDE_PATH
    DEPENDPATH  += $$LIB_INCLUDE_PATH
    export(LIBS)
    export(INCLUDEPATH)
    export(DEPENDPATH)

    debug(Linking: $$LIB_PATH -$$LIB_NAME with include path: $$LIB_INCLUDE_PATH, 1)
}

# Retrieves library deploy path, there are a few different scenarios here:
#
#   - If this file was linked to from a plugin (LIVEKEYS_BIN_PATH is setup), then:
#       - On windows, if we're building Livekeys together with the plugin, then the path is in
#         LIVEKEYS_BIN_PATH/dev/<plugin_path/lib, otherwise the libraries have been deployed in the dev dir
#       - On other systems it's pretty straight forward, LIVEKEYS_BIN_PATH/plugins/<plugin_path> is the actual location
#         of the libraries
#
#   - If this file was linked to from Livekeys:
#       - On windows, it's the 'lib' in the BUILD directory
#       - On other systems it's the same as the deployment directory
#
defineReplace(pluginLibraryDeployPath){
    isEmpty(LIVEKEYS_BIN_PATH){ # File is not included from a plugin
        win32:return($$LIBRARY_DEPLOY_PATH/dev/plugins/$$1/lib)
        else:return($$LIBRARY_DEPLOY_PATH)
    } else {
        isEmpty(LIVEKEYS_DEV_PATH){
            error(LIVEKEYS_BIN_PATH setup without LIVEKEYS_DEV_PATH. Both are required from a plugin.)
        }
        win32{ # On windows, we have a separate location for the libraries if we are building from source
            exists($$LIVEKEYS_DEV_PATH/plugins/$$1/lib): return($$LIVEKEYS_DEV_PATH/plugins/$$1/lib)
            else: return($$LIVEKEYS_BIN_PATH/dev/plugins/$$1/lib)
        } else {
            return($$LIVEKEYS_BIN_PATH/plugins/$$1)
        }
    }
}

# Setup library include path
#
# Depends on whether this file was included from a plugin or from Livekeys
#
defineReplace(libraryIncludePath){
    isEmpty(LIVEKEYS_DEV_PATH):return($$PROJECT_ROOT)
    else:return($$LIVEKEYS_DEV_PATH)
}


# Links a plugin within a specified path to the current project
#
# Args: (path, name, [include_dir])
#  * dependencyPath: path to the dependency
#  * path: path to the plugin from the dependency 'plugins' directory
#  * name: name of the library
#  * include_dir: include dir path(defaults to library path in source tree + '/src')
#
defineTest(linkPlugin){
    LIB_PATH = $$pluginLibraryDeployPath($$1)

    LIB_NAME = $$2
    LIB_INCLUDE_PATH = $$libraryIncludePath()/plugins/$$1/include

    !isEmpty($$3):LIB_INCLUDE_PATH=$$3

    # use *= instead of += to prevent duplications of link path cofigurations
    LIBS *= -L$$LIB_PATH
    LIBS *= -l$$LIB_NAME
    INCLUDEPATH += $$LIB_INCLUDE_PATH
    DEPENDPATH  += $$LIB_INCLUDE_PATH
    export(LIBS)
    export(INCLUDEPATH)
    export(DEPENDPATH)

    debug(Linking: $$LIB_PATH -$$LIB_NAME with include path: $$LIB_INCLUDE_PATH, 1)
}

# Generates the commands required for deploying directories by recursive copy.
# This function takes care of quoting and OS dependent path transformations
# CAUTION: Only the contents of the directory gets copied, not the directory
# itself
#
# Args: (from_dir, to_parent_dir)
#  * from_dir: is the path to the directory whose content is to be copied
#  * to_parent_dir: is the parent directory in which the copy will be placed
#
# Example:
#
# mycopy.command = $$deployDirCommand(foo foo, bar/baz)
# # generates the command
# # cp -f -R 'foo foo/.' bar/baz under Unix systems
#
defineReplace(deployDirCommand){
    DEPLOY_FROM = $$shell_path($$shell_quote($$1/.))
    DEPLOY_TO = $$shell_path($$shell_quote($$2))

    debug(Deploy $$DEPLOY_FROM to $$DEPLOY_TO, 1)

    return($$QMAKE_COPY_DIR $$DEPLOY_FROM $$DEPLOY_TO)
}

# Generates the commands required for deploying single files by copying.
# This function takes care of quoting and OS dependent path transformations
#
# Args: (from_file, to_parent_dir)
#  * from_file: is the path to the file that is to be copied
#  * to_parent_dir: is the parent directory in which the copy will be placed
#
# Example:
#
# mycopy.command = $$deployDirCommand(foo foo, bar/baz)
# # generates the command
# # cp -f 'foo foo' bar/baz under Unix systems
# # and would upon execution generate the file 'bar/baz/foo foo'
#
defineReplace(deployFileCommand){
    DEPLOY_FROM = $$shell_path($$shell_quote($$1))
    DEPLOY_TO = $$shell_path($$shell_quote($$2))

    debug(Deploy $$DEPLOY_FROM to $$DEPLOY_TO, 1)

    return($$QMAKE_COPY_FILE $$DEPLOY_FROM $$DEPLOY_TO)
}

# Checks the qt version
#
# Args: (major, minor)
# Returns: true if version major is the same and version minor is the same or higher
#
defineTest(qtVersionCheck) {
    maj = $$1
    min = $$2
    isEqual(QT_MAJOR_VERSION, $$maj){
        isEqual(QT_MINOR_VERSION, $$min):return(true)
        greaterThan(QT_MINOR_VERSION, $$min):return(true)
    }
    return(false)
}
