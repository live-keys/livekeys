TEMPLATE = subdirs

SUBDIRS += \
    lvbase \
    lveditor \
    lveditqmljs

# --- Subdir configuration ---
lveditor.subdir    = $$PWD/lveditor
lveditqmljs.subdir = $$PWD/lveditqmljs

# --- Dependency configuration ---
lveditqmljs.depends  = lveditor
