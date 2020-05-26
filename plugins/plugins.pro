
# Plugin build configuration,
# !!! Modify only if you know what you are doing

TEMPLATE = subdirs

# Optional build modules

SUBDIRS += \
    base \
    live \
    editor \
    editqml \
    fs \
    workspace \
    squareone \
    timeline \
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
fs.subdir               = $$PWD/fs
lcvphoto.subdir         = $$PWD/lcvphoto
lcvvideo.subdir         = $$PWD/lcvvideo
timeline.subdir         = $$PWD/timeline

# --- Dependency configuration ---

live.depends          = editor
editqml.depends       = live
lcvcore.depends       = live timeline
lcvfeatures2d.depends = lcvcore live
lcvimgproc.depends    = lcvcore live
lcvphoto.depends      = lcvcore live
lcvvideo.depends      = lcvcore live

!isEmpty(BUILD_ELEMENTS){
    SUBDIRS += editlv
    editlv.depends = live
    editlv.subdir = $$PWD/editlv

    SUBDIRS += editjson
    editjson.depends = live
    editjson.subdir = $$PWD/editjson

    SUBDIRS += language
    language.subdir = $$PWD/language

    SUBDIRS += test
    test.subdir = $$PWD/test
}

# Deploy the samples

samplescopy.commands = $$deployDirCommand($$PWD/../samples, $$DEPLOY_PATH/samples)
first.depends = $(first) samplescopy
export(first.depends)
export(samplescopy.commands)
QMAKE_EXTRA_TARGETS += first samplescopy
