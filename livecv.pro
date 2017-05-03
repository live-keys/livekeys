
# --- Bootstrapping the build process ---

CONFIG_FILES = \
    $$CONFIG_DIR/config_functions.pri \
    $$CONFIG_DIR/config_paths.pri

# Check for the existence of the configuration files and import them
for(file, CONFIG_FILES){
    message(Checking for $$file)

    !exists($$file){
        error(Missing config file $$file)
    } else{
        message(Including $$file)
        include($$file)
        message(Include finished)
    }
}

# From now on we can use all the predefined helper functions and variables

# --- Verbosity setting ---
# Comment one of the following
# Note that this option is also propagated to subprojects

# initVar(VERBOSE, true)
  initVar(VERBOSE, false)

# --- Subdir configuration ---

TEMPLATE = subdirs

SUBDIRS += \
    application \
    lib \
    plugins

# Subdiŕ configurations
application.subdir  = $$PATH_SOURCE_APPLICATION
lib.subdir          = $$PATH_SOURCE_LIB
plugins.subdir      = $$PATH_SOURCE_PLUGINS

# Dependency configuration
application.depends = lib
plugins.depend      = lib
