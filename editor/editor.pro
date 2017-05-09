
TEMPLATE = subdirs

SUBDIRS += \
    lcveditor \
    qmljsparser

# --- Subdir configuration ---
lcveditor.subdir    = $$PWD/lcveditor
qmljsparser.subdir  = $$PWD/qmljsparser

# --- Dependency configuration ---
qmljsparser.depends  = lcveditor
