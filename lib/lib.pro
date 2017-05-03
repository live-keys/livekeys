
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

# TODO Clarify: "live" actually seems to be a plugin. Why is it in lib?
# TODO handle "live" properly


TEMPLATE = subdirs

SUBDIRS += \
    live \
    lcveditor \
    qmljsparser

# --- Subdir configuration ---
lcveditor.subdir = $$PATH_SOURCE_LIB_LCVEDITOR
qmljsparser.subdir = $$PATH_SOURCE_LIB_QMLJSPARSER
