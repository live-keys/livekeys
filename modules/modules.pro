# include(Config.pro)

# Add more folders to ship with the application, here
# folder_01.source = qml/LiveCV
# folder_01.target = qml
# folder_02.source = qml/LiveCVImages
# folder_02.target = qml
# DEPLOYMENTFOLDERS += folder_01 folder_02

# Additional import path used to resolve QML modules in Creator's code model
# QML_IMPORT_PATH =

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.

TEMPLATE = subdirs
CONFIG  += ordered

# Required modules

SUBDIRS += $$PWD/application
SUBDIRS += $$PWD/lcvlib

# Optional build modules

SUBDIRS += $$PWD/lcvcore
SUBDIRS += $$PWD/lcvimgproc
SUBDIRS += $$PWD/lcvvideo

