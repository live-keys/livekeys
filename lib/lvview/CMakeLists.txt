
add_library(lvview SHARED)

set_target_properties(lvview PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_DEPLOY_PATH}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBRARY_DEPLOY_PATH}"
)

target_sources(lvview PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/componentdeclaration.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/errorhandler.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/incubationcontroller.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/propertyparserhook.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlbuild.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlerror.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlfunctionsource.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlmetaextension.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlprogram.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlprojectinfo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlsourcelocation.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlstreamprovider.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlvisuallog.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlwritablestream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/settings.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlclipboard.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/viewengineinterceptor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/visuallogbasemodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/visuallogfilter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/visuallogmodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/visuallognetworksender.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mlnodetoqml.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/viewcontext.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/viewengine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlpromise.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/memory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lockedfileiosession.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/shared.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlvariantlist.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlvariantlistmodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlobjectlist.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlobjectlistmodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/linecapture.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/layer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/windowlayer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlpropertywatcher.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlstream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlworkerpool.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sharedmemoryreadworker.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sharedmemorywriteworker.cpp"
)

target_compile_definitions(lvview PRIVATE LV_VIEW_LIB)
target_compile_features(lvview PRIVATE cxx_std_17)

target_include_directories(lvview
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(lvview
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::QuickPrivate )

target_link_libraries(lvview PRIVATE lvbase)
