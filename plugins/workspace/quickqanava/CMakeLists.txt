
add_library(quickqanava SHARED)

set_target_properties(quickqanava PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/workspace/quickqanava"
)

add_output_files(quickqanava_qml        ${CMAKE_CURRENT_SOURCE_DIR}/qml        ${MODULE_DEPLOY_PATH}/workspace/quickqanava          workspace_quickqanava_qml_sources)

target_sources(quickqanava PRIVATE ${workspace_quickqanava_qml_sources})


target_sources(quickqanava PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/QuickContainers/include/qcmContainerModel.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/QuickContainers/include/qcmAbstractContainer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/QuickContainers/src/qcmAbstractContainer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/QuickContainers/src/qcmContainerModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanPlugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanGraphView.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanUtils.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanEdge.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanEdgeItem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanNode.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanNodeItem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanPortItem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanSelectable.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanDraggable.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanDraggableCtrl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanConnector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanBehaviour.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanGraph.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanGroup.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanGroupItem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanStyle.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanStyleManager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanNavigable.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanNavigablePreview.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanGrid.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanLineGrid.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qanBottomRightResizer.cpp"
)

target_include_directories(quickqanava
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/GTpo/src"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/QuickContainers/include"
)

target_link_libraries(quickqanava
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick )

target_link_libraries(quickqanava PRIVATE lvbase)
