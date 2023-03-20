
add_library(lveditor SHARED)

set_target_properties(lveditor PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_DEPLOY_PATH}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBRARY_DEPLOY_PATH}"
)

target_sources(lveditor PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/codecompletionmodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/codecompletionsuggestion.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/codehandler.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/codepalette.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/commands.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/commandsmodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/fileformattypes.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hookcontainer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/document.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/documentation.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/documentationloader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/editorlayer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/editorprivate_plugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/editorsettings.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/extensions.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/keymap.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/linecontrol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/linesurface.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/paletteloader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/project.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/projectdocument.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/projectdocumentmodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/projectengineinterceptor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/projectentry.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/projectfile.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/projectfileindexer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/projectfilemodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/projectworkspace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/runnable.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/runnablecontainer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/startupmodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/syntaxhighlighter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/textdocumentdata.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/texteditnodedebugmodel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/theme.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/themecontainer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/workspace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/workspaceextension.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/workspaceipc.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/workspaceipcsink.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/workspacelayer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/workspacemessagestack.cpp"
)

target_sources(lveditor PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/textcontrol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/nodeengine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/textedit.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/textutil.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/textnode.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/textdocumentlayout.cpp"
)

target_compile_definitions(lveditor PRIVATE LV_EDITOR_LIB)
target_compile_features(lveditor PRIVATE cxx_std_17)

target_include_directories(lveditor
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(lveditor
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::QuickPrivate )

target_link_libraries(lveditor PRIVATE lvbase lvview)
