# Avoid Multiple Inclusion
isEmpty(CONFIG_PRO){
CONFIG_PRO = 1


win32{

# Configuration for Windows
# -------------------------
#
# By default, the configuration considers OPENCV_DIR system environment variable is set. If not, you can either modify
# the following values, or consult the Open CV manual on how to set up Open CV for Visual Studio.
#
# PATH_OPENCV_INCLUDE   : Path to opencv include directory ( Example : C:/opencv/include )
# PATH_OPENCV_LIBRARIES : Path to opencv libraries directory ( Example : C:/opencv/build/x64/vc11/lib )
# PATH_OPENCV_DLLS      : Path to opencv dll files ( Example : C:/opencv/build/x64/vc11/bin )
# VERSION_OPENCV        : Version of opencv used to link lib files and copy dll files to the build directory.

    PATH_OPENCV_INCLUDE   = $$(OPENCV_DIR)/../../include
    PATH_OPENCV_LIBRARIES = $$(OPENCV_DIR)/lib
    PATH_OPENCV_DLLS      = $$(OPENCV_DIR)/bin
    VERSION_OPENCV        = 2410

    CONFIG(debug, debug|release){
        VERSION_OPENCV    = $${VERSION_OPENCV}d
    }

    INCLUDEPATH += $${PATH_OPENCV_INCLUDE}

    CONFIG(debug, debug|release){
        DLL_DESTINATION   = ../application/debug
    }
    CONFIG(release, debug|release){
        DLL_DESTINATION   = ../application/release
    }
    #
    # DEPLOYMENTFOLDERS += opencv_folder

    # Helper Function to copy dlls
    defineTest(copyCvDll) {
            files = $$1

            for(FILE, files) {
                DDIR = $${DLL_DESTINATION}

                # Replace slashes in paths with backslashes for Windows
                win32:FILE ~= s,/,\\,g
                win32:DDIR ~= s,/,\\,g

                QMAKE_POST_LINK += $$QMAKE_COPY \"$$FILE\" \"$$DDIR\" $$escape_expand(\\n\\t)
            }
            export(QMAKE_POST_LINK)
    }

    # opencv_core
    LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_core$${VERSION_OPENCV}
    copyCvDll($${PATH_OPENCV_DLLS}/opencv_core$${VERSION_OPENCV}.dll)

    # opencv_highgui
    !isEmpty(REQUIRES_CV_HIGHGUI){
        LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_highgui$${VERSION_OPENCV}
        copyCvDll($${PATH_OPENCV_DLLS}/opencv_highgui$${VERSION_OPENCV}.dll)
    }

    # opencv_imgproc
    !isEmpty(REQUIRES_CV_IMGPROC){
        LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_imgproc$${VERSION_OPENCV}
        copyCvDll($${PATH_OPENCV_DLLS}/opencv_imgproc$${VERSION_OPENCV}.dll)
    }

    # opencv_video
    !isEmpty(REQUIRES_CV_VIDEO){
        LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_video$${VERSION_OPENCV}
        copyCvDll($${PATH_OPENCV_DLLS}/opencv_video$${VERSION_OPENCV}.dll)
    }

    # opencv_calib3d
    !isEmpty(REQUIRES_CV_CALIB3D){
        LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_calib3d$${VERSION_OPENCV}
        copyCvDll($${PATH_OPENCV_DLLS}/opencv_calib3d$${VERSION_OPENCV}.dll)
    }

    # opencv_features2d
    !isEmpty(REQUIRES_CV_FEATURES2D){
        LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_features2d$${VERSION_OPENCV}
        copyCvDll($${PATH_OPENCV_DLLS}/opencv_features2d$${VERSION_OPENCV}.dll)
    }
}

# Configuration for Unix
# ----------------------
#
# Here it's simpler

unix{
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv

}


}
