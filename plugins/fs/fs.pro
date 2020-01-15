TEMPLATE = subdirs

SUBDIRS += \
    fsqml

!isEmpty(BUILD_ELEMENTS){
    SUBDIRS += fslv
}
