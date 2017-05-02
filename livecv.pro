TEMPLATE = subdirs
#CONFIG  += ordered
#SUBDIRS += $$PWD/editor
#SUBDIRS += $$PWD/application
#SUBDIRS += $$PWD/lib/live.pro
#SUBDIRS += $$PWD/plugins
#SUBDIRS += $$PWD/tests

SUBDIRS += \
    editor \
    application \
    live \
    plugins

# Subdirectory configuration

editor.subdir       = $$PWD/editor
application.subdir  = $$PWD/application
live.subdir         = $$PWD/lib/live
plugins.subdir      = $$PWD/plugins

# dependency configuration
application.depends = live
plugins.depend      = live
