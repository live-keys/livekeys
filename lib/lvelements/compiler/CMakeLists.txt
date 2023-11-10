if(BUILD_LVELEMENTSCOMPILER_STATIC)
    add_library(lvelementscompiler STATIC)
else()
    add_library(lvelementscompiler SHARED)
endif()

set_target_properties(lvelementscompiler PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_DEPLOY_PATH}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBRARY_DEPLOY_PATH}"
)


target_sources(lvelementscompiler PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cursorcontext.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/elementsmodule.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/languageinfo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/elementssections.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/languagenodes.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/languagenodestojs.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/languageparser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/modulefile.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parseddocument.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sourcerange.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/languagenodeinfo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/propertybindingcontainer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/languagequery.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tracepointexception.cpp"
)

target_include_directories(lvelementscompiler
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions(lvelementscompiler PRIVATE LV_ELEMENTS_COMPILER_LIB)

target_link_libraries(lvelementscompiler PRIVATE lvbase)

if(BUILD_LVBASE_STATIC)
    target_compile_definitions(lvelementscompiler PRIVATE LV_BASE_STATIC)
endif()
if(BUILD_LVELEMENTSCOMPILER_STATIC)
    target_compile_definitions(lvelementscompiler PRIVATE LV_ELEMENTS_COMPILER_STATIC)
endif()

# Include 3rdparty library - treesitter

target_sources(lvelementscompiler
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/treesitter/lib/src/lib.c"
)
target_include_directories(lvelementscompiler
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/treesitter/lib/src"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/treesitter/lib/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/treesitter/utf8proc"
)

# Include library - tree-sitter-elements

target_sources(lvelementscompiler
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/treesitterelements/elementsparserinternal.c"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/treesitterelements/scanner.c"
)

target_include_directories(lvelementscompiler
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/treesitterelements"
)

if(BUILD_TESTS)
    add_subdirectory(test/unit)
endif()
