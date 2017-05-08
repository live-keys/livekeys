
# Plugin build configuration,
# !!! Modify only if you know what you are doing

TEMPLATE = subdirs

# Optional build modules

SUBDIRS += \
    lcvcore \
    lcvimgproc \
    lcvfeatures2d \
    live \
    lcvphoto \
    lcvvideo

# Subdir configuration

lcvcore.subdir          = $$PATH_SOURCE_PLUGINS_CORE
lcvimgproc.subdir       = $$PATH_SOURCE_PLUGINS_IMGPROC
lcvfeatures2d.subdir    = $$PATH_SOURCE_PLUGINS_FEATURES2D
live.subdir             = $$PATH_SOURCE_PLUGINS_LIVE
lcvphoto.subdir         = $$PATH_SOURCE_PLUGINS_PHOTO
lcvvideo.subdir         = $$PATH_SOURCE_PLUGINS_VIDEO

# Dependency configuration

lcvcore.depends         = live
lcvfeatures2d.depends   = lcvcore live
lcvimgproc.depends      = lcvcore live
