TEMPLATE = subdirs
SUBDIRS += $$PWD/lvbasetest
SUBDIRS += $$PWD/lvviewtest
SUBDIRS += $$PWD/lveditortest

!isEmpty(BUILD_ELEMENTS_ENGINE){
    SUBDIRS += $$PWD/lvelementstest
}

