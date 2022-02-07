TEMPLATE = subdirs

SUBDIRS += \
    baseqml

!isEmpty(BUILD_ELEMENTS_ENGINE){
    SUBDIRS += baselv
}

