TEMPLATE = subdirs
SUBDIRS += $$PWD/lvbasetest
SUBDIRS += $$PWD/lvviewtest

!isEmpty(BUILD_ELEMENTS){
    SUBDIRS += $$PWD/lvelementstest
}

