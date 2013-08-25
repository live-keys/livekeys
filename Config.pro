# Avoid Multiple Inclusion
isEmpty(CONFIG_PRO){
CONFIG_PRO = 1

# Configuration for Windows
# -------------------------
#
# PATH_OPENCV_INCLUDE   : Path to opencv include directory ( Example : C:/opencv/include )
# PATH_OPENCV_LIBRARIES : Path to opencv libraries directory ( Example : C:/opencv/build/x64/vc11/lib )
# PATH_OPENCV_DLLS      : Path to opencv dll files ( Example : C:/opencv/build/x64/vc11/bin )
# VERSION_OPENCV        : Version of opencv used. This can become helpful when changing opencv versions.
#                         If you're using visual studio, you can set up opencv version according to debug
#                         or release mode.

win32{

	PATH_OPENCV_INCLUDE   = C:/opencv/build/include
	PATH_OPENCV_LIBRARIES = C:/opencv/build/x64/vc11/lib
	PATH_OPENCV_DLLS      = C:/opencv/build/x64/vc11/bin

	debug{
		VERSION_OPENCV    = 244d
	}
	release{
		VERSION_OPENCV    = 244
	}


	INCLUDEPATH += $${PATH_OPENCV_INCLUDE}

	LIBS += -L$${PATH_OPENCV_LIBRARIES}/ \
		-lopencv_core$${VERSION_OPENCV} \
		-lopencv_highgui$${VERSION_OPENCV} \
		-lopencv_imgproc$${VERSION_OPENCV} \
		-lopencv_features2d$${VERSION_OPENCV} \
		-lopencv_calib3d$${VERSION_OPENCV} \
		-lopencv_video$${VERSION_OPENCV}

	opencv_folder.source = $${PATH_OPENCV_DLLS}
	debug{
		opencv_folder.target = debug
	}
	release{
		opencv_folder.target = release
	}
	#
	# DEPLOYMENTFOLDERS += opencv_folder
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
