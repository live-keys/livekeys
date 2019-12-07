TEMPLATE = subdirs

SUBDIRS += \
    lvbase \
    lvview \
    lveditor \
    lveditqmljs


# --- Subdir configuration ---
lvbase.subdir      = $$PWD/lvbase
lvview.subdir      = $$PWD/lvview
lveditor.subdir    = $$PWD/lveditor
lveditqmljs.subdir = $$PWD/lveditqmljs


# --- Dependency configuration --
lvview.depends = lvbase
lveditor.depends = lvview
lveditqmljs.depends  = lveditor


!isEmpty(BUILD_ELEMENTS){
    SUBDIRS += lvelements
    lvelements.subdir  = $$PWD/lvelements
    lvelements.depends = lvbase
}
