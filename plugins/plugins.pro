
# Plugin build configuration,
# !!! Modify only if you know what you are doing

TEMPLATE = subdirs

# Optional build modules

SUBDIRS += \
    live \
    editor \
    editqml \
    lcvcore \
    lcvimgproc \
    lcvfeatures2d \
    lcvphoto \
    lcvvideo

# --- Subdir configuration ---

lcvcore.subdir          = $$PWD/lcvcore
lcvimgproc.subdir       = $$PWD/lcvimgproc
lcvfeatures2d.subdir    = $$PWD/lcvfeatures2d
live.subdir             = $$PWD/live
editor.subdir           = $$PWD/editor
editqml.subdir          = $$PWD/editqml
lcvphoto.subdir         = $$PWD/lcvphoto
lcvvideo.subdir         = $$PWD/lcvvideo

# --- Dependency configuration ---

live.depends          = editor
editqml.depends       = live
lcvcore.depends       = live
lcvfeatures2d.depends = lcvcore live
lcvimgproc.depends    = lcvcore live
lcvphoto.depends      = lcvcore live
lcvvideo.depends      = lcvcore live
