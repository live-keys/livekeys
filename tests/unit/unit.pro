TEMPLATE = subdirs
SUBDIRS += $$PWD/lvbasetest
SUBDIRS += $$PWD/lvviewtest
SUBDIRS += $$PWD/lveditortest

!isEmpty(BUILD_ELEMENTS){
    SUBDIRS += $$PWD/lvelementstest
}

