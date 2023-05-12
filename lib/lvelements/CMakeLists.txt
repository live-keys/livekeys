
add_library(lvelements SHARED)

set_target_properties(lvelements PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_DEPLOY_PATH}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBRARY_DEPLOY_PATH}"
)


target_sources(lvelements PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler/compiler.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler/elementsmodule.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler/elementssections.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler/languagenodes.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler/languagenodestojs.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler/languageparser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler/modulefile.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler/sourcerange.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler/languagenodeinfo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler/propertybindingcontainer.cpp"
)

target_include_directories(lvelements
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions(lvelements PRIVATE LV_ELEMENTS_LIB)

target_link_libraries(lvelements PRIVATE lvbase)

# Include 3rdparty library - treesitter

target_sources(lvelements
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/treesitter/lib/src/lib.c"
)
target_include_directories(lvelements
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/treesitter/lib/src"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/treesitter/lib/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/treesitter/utf8proc"
)

# Include library - tree-sitter-elements

target_sources(lvelements
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/treesitterelements/elementsparserinternal.c"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/treesitterelements/scanner.c"
)

target_include_directories(lvelements
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/treesitterelements"
)

if(SHOULD_BUILD_ELEMENTS_ENGINE)
    target_compile_definitions(lvelements PRIVATE BUILD_ELEMENTS_ENGINE)

    target_include_directories(lvelements PRIVATE ${V8_INCLUDE_DIRS})

    target_link_libraries(lvelements PRIVATE ${V8_LIBS})

    target_sources(lvelements PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/bindingpath.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/cursorcontext.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/function.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/engine.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/jsmodule.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/languageinfo.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/languagequery.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/languagescanner.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/metaobject.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/modulefilejsdata.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/parseddocument.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/value.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/element.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/property.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/event.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/eventfunction.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/context.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/script.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/component.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/constructor.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/callable.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/object.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/list.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/buffer.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/container.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/errorhandler.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/mlnodetojs.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/visuallogjsobject.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/modulelibrary.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/tuple.cpp"
    )
endif()

