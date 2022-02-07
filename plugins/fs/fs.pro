TEMPLATE = subdirs

SUBDIRS += \
    fsqml

!isEmpty(BUILD_ELEMENTS_ENGINE){
    SUBDIRS += fslv
}
