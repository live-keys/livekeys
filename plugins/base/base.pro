TEMPLATE = subdirs

SUBDIRS += \
    baseqml

!isEmpty(BUILD_ELEMENTS){
    SUBDIRS += baselv
}

