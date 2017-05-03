
# Plugin build configuration,
# !!! Modify only if you know what you are doing

# Plugins depend on the "live" library.
# This dependency is already taken care of in livecv.pro

TEMPLATE = subdirs

# Optional build modules

SUBDIRS += \
    lcvcore \
    lcvimgproc \
    lcvfeatures2d \
    lcvphoto \
    lcvvideo

# Subdir configuration

lcvcore.subdir          = $$PWD/lcvcore
lcvimgproc.subdir       = $$PWD/lcvimgproc
lcvfeatures2d.subdir    = $$PWD/lcvfeatures2d
lcvphoto.subdir         = $$PWD/lcvphoto
lcvvideo.subdir         = $$PWD/lcvvideo

# Dependency configuration

lcvimgproc.depend       = lcvcore
lcvfeatures2d.depend    = lcvcore
