# Avoid Multiple Inclusion
isEmpty(OPENCVCONFIG_PRO){
OPENCVCONFIG_PRO = 1

win32{

    # Windows Configuration

    # By default, we consider the OPENCV_DIR system environment variable set. The value is set up when installing
    # Open CV for Visual Studio. The variables can also be configured from below.

    OPENCV_DIR_INCLUDE   = $$(OPENCV_DIR)/../../include
    OPENCV_DIR_LIBRARIES = $$(OPENCV_DIR)/lib
    OPENCV_DIR_DLLS      = $$(OPENCV_DIR)/bin

    # Determine Version

    isEmpty(OPENCV_VERSION){

        OPENCV_VERSION_FIND = $$files($$OPENCV_DIR_LIBRARIES/opencv_world*)
        OPENCV_VERSION_FIND = $$first(OPENCV_VERSION_FIND)

        OPENCV_VERSION_FIND = $$split(OPENCV_VERSION_FIND, opencv_world)
        OPENCV_VERSION_FIND = $$last(OPENCV_VERSION_FIND)

        OPENCV_VERSION_FIND = $$replace(OPENCV_VERSION_FIND, [^0-9], '')
        OPENCV_VERSION      = $$OPENCV_VERSION_FIND
    }

    CONFIG(debug, debug|release):OPENCV_VERSION = $${OPENCV_VERSION}d

    INCLUDEPATH += $${OPENCV_DIR_INCLUDE}

    OPENCV_DLL_DESTINATION = $$DEPLOY_PATH/external/opencv

    # Helper Function to copy dlls

    defineTest(copyCvDll) {
        files = $$1

        for(FILE, files) {
            DDIR = $${OPENCV_DLL_DESTINATION}

            # Replace slashes in paths with backslashes
            win32:FILE ~= s,/,\\,g
            win32:DDIR ~= s,/,\\,g

            QMAKE_POST_LINK += $$QMAKE_COPY \"$$FILE\" \"$$DDIR\" $$escape_expand(\\n\\t)
        }
        export(QMAKE_POST_LINK)
    }

    defineTest(mkCvDir){
        DDIR = $$1
        !exists($$DDIR){
            win32:DDIR ~= s,/,\\,g
            QMAKE_POST_LINK += $(CHK_DIR_EXISTS) \"$${DDIR}\" $(MKDIR) \"$${DDIR}\" $$escape_expand(\n\t)
            export(QMAKE_POST_LINK)
        }
    }

    LIBS += -L$${OPENCV_DIR_LIBRARIES} -lopencv_world$${OPENCV_VERSION}

    defineTest(deployOpenCV){
        mkCvDir($$DEPLOY_PATH/external)
        mkCvDir($$DEPLOY_PATH/external/opencv)
        copyCvDll($${OPENCV_DIR_DLLS}/opencv_world$${OPENCV_VERSION}.dll)
        copyCvDll($${OPENCV_DIR_DLLS}/opencv_videoio_ffmpeg$${OPENCV_VERSION_FIND}_64.dll)
    }
}

unix{

    # Unix Configuration

    macx:QT_CONFIG -= no-pkg-config

    CONFIG += link_pkgconfig
    PKGCONFIG += opencv4

    defineTest(deployOpenCV){}

}


}
