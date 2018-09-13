TEMPLATE = subdirs
SUBDIRS += $$PWD/lvbasetest

!isEmpty(BUILD_ELEMENTS){
    SUBDIRS += $$PWD/lvelementstest
}

