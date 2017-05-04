PLUGIN_NAME = lcvfeatures2d
PLUGIN_QML_DIR = $$PATH_SOURCE_PLUGINS_FEATURES2D/qml
include($$getConfigFile(is_plugin.pri))
include($$getConfigFile(use_plugin_core.pri))
include($$getConfigFile(use_plugin_live.pri))

#uri = plugins.lcvfeatures2d

DEFINES += Q_LCV
DEFINES += Q_LCVFEATURES2D_LIB

include($$PWD/src/lcvfeatures2d.pri)
include($$PWD/include/lcvfeatures2dheaders.pri)
include($$PWD/../../3rdparty/opencvconfig.pro)

deployOpenCV()

OTHER_FILES += \
    $$PWD/qml/KeypointListView.qml \
    $$PWD/qml/FeatureDetectorSelection.qml \
    $$PWD/qml/SelectionArea.qml \
    $$PWD/qml/SelectionWindow.qml \
    $$PWD/qml/FeatureObjectList.qml \
    $$PWD/qml/FeatureObjectMatch.qml \
    $$PWD/qml/DescriptorExtractorSelection.qml
    $$PWD/qml/qmldir
