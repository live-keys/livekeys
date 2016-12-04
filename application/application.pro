TEMPLATE = app
TARGET   = livecv
CONFIG  += c++11
QT      += qml quick

# Editor library
# --------------

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../editor/lcveditor/release/ -llcveditor
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../editor/lcveditor/debug/ -llcveditor
else:unix: LIBS += -L$$OUT_PWD/../application/lcveditor/ -llcveditor

INCLUDEPATH += $$PWD/../editor/lcveditor/src
DEPENDPATH += $$PWD/../editor/lcveditor/src

# QmlJS library
# -------------

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../editor/qmljsparser/release/ -lqmljsparser
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../editor/qmljsparser/debug/ -lqmljsparser
else:unix: LIBS += -L$$OUT_PWD/../application/qmljsparser/ -lqmljsparser

INCLUDEPATH += $$PWD/../editor/qmljsparser/src
DEPENDPATH += $$PWD/../editor/qmljsparser/src

# Application
# -----------

include($$PWD/src/base/base.pri)
include($$PWD/src/main/main.pri)

RC_ICONS = $$PWD/icons/livecv.ico

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

# Deploy Plugins
# --------------

PLUGIN_DEPLOY_FROM = $$PWD/plugins
win32:CONFIG(debug, debug|release): PLUGIN_DEPLOY_TO = $$OUT_PWD/../application/debug/plugins
else:win32:CONFIG(release, debug|release): PLUGIN_DEPLOY_TO = $$OUT_PWD/../application/release/plugins
else:unix: PLUGIN_DEPLOY_TO = $$OUT_PWD/../application

win32:PLUGIN_DEPLOY_TO ~= s,/,\\,g
win32:PLUGIN_DEPLOY_FROM ~= s,/,\\,g

plugincopy.commands = $(COPY_DIR) \"$$PLUGIN_DEPLOY_FROM\" \"$$PLUGIN_DEPLOY_TO\"

first.depends = $(first) plugincopy
export(first.depends)
export(plugincopy.commands)

QMAKE_EXTRA_TARGETS += first plugincopy

# Qml
# ---

RESOURCES += $$PWD/application.qrc

OTHER_FILES += \
    $$PWD/plugins/lcvcontrols/RegionSelection.qml \
    $$PWD/plugins/lcvcontrols/VideoControls.qml \
    $$PWD/plugins/lcvcontrols/KeypointListView.qml \
    $$PWD/plugins/lcvcontrols/LiveCVScrollStyle.qml \
    $$PWD/plugins/lcvcontrols/qmldir

include(deployment.pri)

DISTFILES += \
    $$PWD/plugins/lcvcontrols/DropDown.qml \
    $$PWD/plugins/lcvcontrols/ConfigurationPanel.qml \
    $$PWD/plugins/lcvcontrols/ConfigurationField.qml \
    $$PWD/plugins/lcvcontrols/InputBox.qml \
    plugins/lcvcontrols/FeatureDetectorSelection.qml \
    plugins/lcvcontrols/SelectionArea.qml \
    $$PWD/plugins/lcvcontrols/SelectionWindow.qml \
    plugins/lcvcontrols/LiveCVStyle.qml \
    plugins/lcvcontrols/FeatureObjectList.qml \
    plugins/lcvcontrols/FeatureObjectMatch.qml \
    plugins/lcvcontrols/DescriptorExtractorSelection.qml \
    plugins/lcvcontrols/TextButton.qml

