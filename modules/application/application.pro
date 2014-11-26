TEMPLATE = app
TARGET   = livecv


# Shipment folders with the application
# -------------------------------------

win32:CONFIG(debug, debug|release): DEPLOY_PATH = debug
else:win32:CONFIG(release, debug|release): DEPLOY_PATH = release
else:unix: DEPLOY_PATH = ""


folder_01.source = images
folder_01.target = $${DEPLOY_PATH}
folder_02.source = plugins
folder_02.target = $${DEPLOY_PATH}
DEPLOYMENTFOLDERS += folder_01 folder_02

# Application Viewer
# ------------------

include(qtquickviewer/qtquick2applicationviewer.pri)
include(src/application.pri)
qtcAddDeployment()

RC_ICONS = livecv.ico

win32{
    Release:DESTDIR = release/../release #fix for app current path
    Release:OBJECTS_DIR = release/.obj
    Release:MOC_DIR = release/.moc
    Release:RCC_DIR = release/.rcc

    Debug:DESTDIR = debug/../debug #fix for app current path
    Debug:OBJECTS_DIR = debug/.obj
    Debug:MOC_DIR = debug/.moc
    Debug:RCC_DIR = debug/.rcc
}

OTHER_FILES += \
    plugins/lcvcontrols/RegionSelection.qml

