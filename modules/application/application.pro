TEMPLATE = app
TARGET   = livecv


# Shipment folders with the application
# -------------------------------------

CONFIG(debug, debug|release){
    folder_01.source = images
    folder_01.target = debug
    folder_02.source = plugins
    folder_02.target = debug
    DEPLOYMENTFOLDERS += folder_01 folder_02
}

CONFIG(release, debug|release){
    folder_01.source = images
    folder_01.target = release
    folder_02.source = plugins
    folder_02.target = release
    DEPLOYMENTFOLDERS += folder_01 folder_02
}

# Application Viewer
# ------------------

include(qtquickviewer/qtquick2applicationviewer.pri)
include(src/application.pri)
qtcAddDeployment()

RC_ICONS = livecv.ico

Release:DESTDIR = release/../release #fix for app current path
Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR = release/.moc
Release:RCC_DIR = release/.rcc

Debug:DESTDIR = debug/../debug #fix for app current path
Debug:OBJECTS_DIR = debug/.obj
Debug:MOC_DIR = debug/.moc
Debug:RCC_DIR = debug/.rcc

