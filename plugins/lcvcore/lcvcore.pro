PLUGIN_NAME = lcvcore
PLUGIN_PATH = lcvcore

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

uri = lcvcore

DEFINES += Q_LCV
DEFINES += Q_LCVCORE_LIB

# Dependencies

linkLocalLibrary(lvbase,      lvbase)
linkLocalLibrary(lvview,      lvview)
linkLocalLibrary(lveditor,    lveditor)
linkLocalLibrary(lveditqmljs, lveditqmljs)

linkLocalPlugin(base, base, base/baseqml)
linkLocalPlugin(live, live)
linkLocalPlugin(timeline, timeline)

# Source

unix:!macx{
    QMAKE_LFLAGS += \
	'-Wl,-rpath,\'\$$ORIGIN/../../link\''

    createlinkdir.commands += $${QMAKE_MKDIR_CMD} $$shell_path($${DEPLOY_PATH}/link)
    QMAKE_EXTRA_TARGETS    += createlinkdir
    POST_TARGETDEPS        += createlinkdir
}

include($$PWD/src/lcvcore.pri)
include($$PWD/include/lcvcoreheaders.pri)
include($$PROJECT_ROOT/project/3rdparty/opencv.pri)
deployOpenCV()

OTHER_FILES *= \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes


# Handling the palettes

OTHER_FILES *= \
    palettes/*.qml \
    samples/*.qml \
    doc/*.md

# Deploy the palettes and samples

palettecopy.commands = $$deployDirCommand($$PWD/palettes, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_NAME/palettes)
samplescopy.commands = $$deployDirCommand($$PWD/samples, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH/samples)
first.depends = $(first) palettecopy samplescopy
export(first.depends)
export(samplescopy.commands)
export(palettecopy.commands)
QMAKE_EXTRA_TARGETS += first palettecopy samplescopy

DISTFILES += \
    palettes/DrawPalette.qml \
    palettes/GrayscaleViewPalette.json \
    palettes/PaperSurfacePalette.qml \
    palettes/TransformPalette.qml \
    palettes/VideoDecoderViewPalette.json \
    palettes/VideoFilePalette.qml \
    palettes/VideoPlayerPalette.qml \
    qml/BlankImage.qml \
    qml/BrushTool.qml \
    qml/ColorHistogramView.qml \
    qml/Crop.qml \
    qml/CropTool.qml \
    qml/DrawSurface.qml \
    qml/EditCvExtension.qml \
    qml/GradientTool.qml \
    qml/GrayscaleView.qml \
    qml/ImageRead.qml \
    qml/ImageSegmentCreator.qml \
    qml/MatViewPane.qml \
    qml/NavigableImageView.qml \
    qml/PaperSurface.qml \
    qml/Perspective.qml \
    qml/PerspectiveTool.qml \
    qml/ScriptVideoSegmentCreator.qml \
    qml/TransformImage.qml \
    qml/VideoCaptureSegmentCreator.qml \
    qml/VideoCaptureSegmentEditor.qml \
    qml/VideoCaptureSegmentFactory.qml \
    qml/VideoDecoderView.qml \
    qml/VideoSurfaceCreator.qml \
    qml/VideoSurfaceFactory.qml \
    qml/VideoSurfaceView.qml \
    qml/VideoTrackExtension.qml \
    qml/VideoTrackFactory.qml \
    qml/live.package.json \
    qml/live.plugin.json

