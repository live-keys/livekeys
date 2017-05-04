# TODO Clarify: "live" actually seems to be a plugin. Why is it in lib?
# TODO handle "live" properly


TEMPLATE = subdirs

SUBDIRS += \
    lcveditor \
    qmljsparser

# --- Subdir configuration ---
lcveditor.subdir    = $$PATH_SOURCE_EDITOR_LCVEDITOR
qmljsparser.subdir  = $$PATH_SOURCE_EDITOR_QMLJSPARSER

# --- Dependency configuration ---
qmljsparser.depend  = lcveditor
