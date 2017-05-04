# This file contains the basic configuration for all editor libraries

!defined(LIB_NAME, var): \
    error($$_FILE_: Tried to set up an editor library without defining LIB_NAME first.)

isEmpty(LIB_NAME): \
    error($$_FILE_: Tried to set up an editor library with an empty LIB_NAME.)


message(Configuring editor library $$LIB_NAME)

TEMPLATE = lib
QT      += core qml quick
CONFIG  += qt c++11
TARGET = $$LIB_NAME
DESTDIR = $$PATH_DEPLOY_APPLICATION
