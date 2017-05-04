
# Plugin build configuration,
# !!! Modify only if you know what you are doing

TEMPLATE = subdirs

# Optional build modules

SUBDIRS += \
    lcvcore \
    lcvimgproc \
    lcvfeatures2d \
    lcvlive \
    lcvphoto \
    lcvvideo

# Subdir configuration

lcvcore.subdir          = $$PATH_SOURCE_PLUGINS_CORE
lcvimgproc.subdir       = $$PATH_SOURCE_PLUGINS_IMGPROC
lcvfeatures2d.subdir    = $$PATH_SOURCE_PLUGINS_FEATURES2D
lcvlive.subdir          = $$PATH_SOURCE_PLUGINS_LIVE
lcvphoto.subdir         = $$PATH_SOURCE_PLUGINS_PHOTO
lcvvideo.subdir         = $$PATH_SOURCE_PLUGINS_VIDEO

# Dependency configuration

lcvcore.depends         = lcvlive
lcvfeatures2d.depend    = lcvcore lcvlive
lcvimgproc.depend       = lcvcore lcvlive
