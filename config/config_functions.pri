
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
        # message($$1 already defined) # TODO better use a proper debug level
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

# Returns '/debug', '/release' or '' according to the build mode

defineReplace(buildModePathExtension){
    if($$USE_BUILD_MODE_PATHS){
        CONFIG(release, debug|release): return(/release)
        else: CONFIG(debug, debug|release): return(/debug)
    }
    return("")
}

# Appends '/debug', '/release' or '' to the path according to the build mode
# and returns the new path

defineReplace(buildModePath){
    return($$1$$buildModePathExtension())
}

# Links a local library to the current project
#
# Args: (path, name, [include_dir])
#  * path: relative path to the library from root
#  * name: name of the library
#  * include_dir: include dir path(defaults to library path in source tree + '/src')

defineTest(linkLocalLibrary){

    win32:LIB_PATH = $$BUILD_PWD/lib
    else:LIB_PATH = $$DEPLOY_PWD

    LIB_NAME = $$2
    LIB_INCLUDE_PATH = $$PROJECT_ROOT/$$1/src
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
#  * path: path to the library from root
#  * name: name of the library
#  * include_dir: include dir path(defaults to library path in source tree + '/src')
#
defineTest(linkLocalPlugin){

    win32:LIB_PATH = $$BUILD_PWD/lib/plugins/$$1
    else:LIB_PATH = $$PATH_DEPLOY_PLUGINS/$$1

    LIB_NAME = $$2
    LIB_INCLUDE_PATH = $$PROJECT_ROOT/plugins/$$1/src
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


defineTest(qtVersionCheck) {
    maj = $$1
    min = $$2
    isEqual(QT_MAJOR_VERSION, $$maj){
        isEqual(QT_MINOR_VERSION, $$min):return(true)
        greaterThan(QT_MINOR_VERSION, $$min):return(true)
    }
    return(false)
}
