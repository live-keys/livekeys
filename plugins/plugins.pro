
# Plugin build configuration,
# !!! Modify only if you know what you are doing

TEMPLATE = subdirs

# Optional build modules

SUBDIRS += \
#    lcvcore \
#    lcvimgproc \
#    lcvfeatures2d \
    live #\
#    lcvphoto \
#    lcvvideo

# --- Subdir configuration ---

lcvcore.subdir          = $$PWD/lcvcore
lcvimgproc.subdir       = $$PWD/lcvimgproc
lcvfeatures2d.subdir    = $$PWD/lcvfeatures2d
live.subdir             = $$PWD/live
lcvphoto.subdir         = $$PWD/lcvphoto
lcvvideo.subdir         = $$PWD/lcvvideo

# --- Dependency configuration ---

lcvcore.depends       = live
lcvfeatures2d.depends = lcvcore live
lcvimgproc.depends    = lcvcore live
