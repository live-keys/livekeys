
add_library(lcvcore SHARED)

set_target_properties(lcvcore PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/lcvcore"
)

if(APPLE)
    set_target_properties(lcvcore PROPERTIES LINK_FLAGS
        "-Wl,-rpath,@executable_path/../Link -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@executable_path/../Frameworks/Live.framework/Libraries")
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(lcvcore PROPERTIES LINK_FLAGS
        "-Wl,-rpath,$$ORIGIN/../../link"
    )
endif()


add_output_files(lcvcore_qml        ${CMAKE_CURRENT_SOURCE_DIR}/qml        ${MODULE_DEPLOY_PATH}/lcvcore          lcvcore_qml_sources)
add_output_files(lcvcore_palettes   ${CMAKE_CURRENT_SOURCE_DIR}/palettes   ${MODULE_DEPLOY_PATH}/lcvcore/palettes lcvcore_palettes_sources)

target_sources(lcvcore PRIVATE ${lcvcore_qml_sources})
target_sources(lcvcore PRIVATE ${lcvcore_palettes_sources})

if(BUILD_PACKAGE_SAMPLES)
    add_output_files(lcvcore_sample_coloradjustments ${CMAKE_CURRENT_SOURCE_DIR}/samples/coloradjustments ${MODULE_DEPLOY_PATH}/lcvcore/samples/coloradjustments     lcvcore_sample_coloradjustments_sources)
    add_output_files(lcvcore_sample_coloradjustments_res ${CMAKE_CURRENT_SOURCE_DIR}/samples/coloradjustments/resources ${MODULE_DEPLOY_PATH}/lcvcore/samples/coloradjustments/resources     lcvcore_sample_coloradjustments_res_sources)
    target_sources(lcvcore PRIVATE ${lcvcore_sample_coloradjustments_sources})
    target_sources(lcvcore PRIVATE ${lcvcore_sample_coloradjustments_res_sources})

    add_output_files(lcvcore_sample_drawing ${CMAKE_CURRENT_SOURCE_DIR}/samples/drawing ${MODULE_DEPLOY_PATH}/lcvcore/samples/drawing     lcvcore_sample_drawing_sources)
    add_output_files(lcvcore_sample_drawing_res ${CMAKE_CURRENT_SOURCE_DIR}/samples/drawing/resources ${MODULE_DEPLOY_PATH}/lcvcore/samples/drawing/resources     lcvcore_sample_drawing_res_sources)
    target_sources(lcvcore PRIVATE ${lcvcore_sample_drawing_sources})
    target_sources(lcvcore PRIVATE ${lcvcore_sample_drawing_res_sources})

    add_output_files(lcvcore_sample_groupingtransformations ${CMAKE_CURRENT_SOURCE_DIR}/samples/groupingtransformations ${MODULE_DEPLOY_PATH}/lcvcore/samples/groupingtransformations     lcvcore_sample_groupingtransformations_sources)
    add_output_files(lcvcore_sample_groupingtransformations_res ${CMAKE_CURRENT_SOURCE_DIR}/samples/groupingtransformations/resources ${MODULE_DEPLOY_PATH}/lcvcore/samples/groupingtransformations/resources     lcvcore_sample_groupingtransformations_res_sources)
    target_sources(lcvcore PRIVATE ${lcvcore_sample_groupingtransformations_sources})
    target_sources(lcvcore PRIVATE ${lcvcore_sample_groupingtransformations_res_sources})

    add_output_files(lcvcore_sample_imagetransformations ${CMAKE_CURRENT_SOURCE_DIR}/samples/imagetransformations ${MODULE_DEPLOY_PATH}/lcvcore/samples/imagetransformations     lcvcore_sample_imagetransformations_sources)
    add_output_files(lcvcore_sample_imagetransformations_res ${CMAKE_CURRENT_SOURCE_DIR}/samples/imagetransformations/resources ${MODULE_DEPLOY_PATH}/lcvcore/samples/imagetransformations/resources     lcvcore_sample_imagetransformations_res_sources)
    target_sources(lcvcore PRIVATE ${lcvcore_sample_imagetransformations_sources})
    target_sources(lcvcore PRIVATE ${lcvcore_sample_imagetransformations_res_sources})

    add_output_files(lcvcore_sample_multiimage ${CMAKE_CURRENT_SOURCE_DIR}/samples/multiimage ${MODULE_DEPLOY_PATH}/lcvcore/samples/multiimage     lcvcore_sample_multiimage_sources)
    add_output_files(lcvcore_sample_multiimages_res ${CMAKE_CURRENT_SOURCE_DIR}/samples/multiimage/resources ${MODULE_DEPLOY_PATH}/lcvcore/samples/multiimage/resources     lcvcore_sample_multiimage_res_sources)
    target_sources(lcvcore PRIVATE ${lcvcore_sample_multiimage_sources})
    target_sources(lcvcore PRIVATE ${lcvcore_sample_multiimage_res_sources})

    add_output_files(lcvcore_sample_videocoloradjustments ${CMAKE_CURRENT_SOURCE_DIR}/samples/videocoloradjustments ${MODULE_DEPLOY_PATH}/lcvcore/samples/videocoloradjustments     lcvcore_sample_videocoloradjustments_sources)
    add_output_files(lcvcore_sample_videocoloradjustments_res ${CMAKE_CURRENT_SOURCE_DIR}/samples/videocoloradjustments/resources ${MODULE_DEPLOY_PATH}/lcvcore/samples/videocoloradjustments/resources     lcvcore_sample_videocoloradjustments_res_sources)
    target_sources(lcvcore PRIVATE ${lcvcore_sample_videocoloradjustments_sources})
    target_sources(lcvcore PRIVATE ${lcvcore_sample_videocoloradjustments_res_sources})

    add_output_files(lcvcore_sample_videotimeline ${CMAKE_CURRENT_SOURCE_DIR}/samples/videotimeline ${MODULE_DEPLOY_PATH}/lcvcore/samples/videotimeline     lcvcore_sample_videotimeline_sources)
    add_output_files(lcvcore_sample_videotimeline_res ${CMAKE_CURRENT_SOURCE_DIR}/samples/videotimeline/resources ${MODULE_DEPLOY_PATH}/lcvcore/samples/videotimeline/resources     lcvcore_sample_videotimeline_res_sources)
    target_sources(lcvcore PRIVATE ${lcvcore_sample_videotimeline_sources})
    target_sources(lcvcore PRIVATE ${lcvcore_sample_videotimeline_res_sources})

endif()

target_sources(lcvcore PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qumat.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmat.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmatop.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qwritablemat.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cvextras.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/imagesegment.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmatdisplay.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qcamcapture.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qcamcapturethread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmatnode.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmatshader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmatstate.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/videosegment.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/videotrack.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/videosurface.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qvideowriter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qvideowriterthread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/scriptvideosegment.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmatio.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qvideodecoder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qvideodecodethread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qdrawhistogram.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qgradient.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qimageview.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qimread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmatread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmatview.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qcolorhistogram.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qimagefile.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qitemcapture.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lcvcore_plugin.cpp"
)

target_include_directories(lcvcore
    PRIVATE ${OpenCV_INCLUDE_DIRS}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_definitions(lcvcore PRIVATE Q_LCVCORE_LIB Q_LCV)

target_link_libraries(lcvcore
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick ${OpenCV_LIBS})

target_link_libraries(lcvcore PRIVATE lvbase lvview lveditor lveditqmljs base live timeline)
