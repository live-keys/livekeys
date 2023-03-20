add_library(lvbase SHARED)

set_target_properties(lvbase PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_DEPLOY_PATH}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBRARY_DEPLOY_PATH}"
)

target_sources(lvbase PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/applicationcontext.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/bytebuffer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/commandlineparser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/datetime.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/directory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/exception.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/fileio.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/library.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/libraryloadpath.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mlnode.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mlnodetojson.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/module.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/package.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/packagegraph.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/palettecontainer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/path.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/program.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/stacktrace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/typename.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utf8.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/visuallog.cpp"
)

target_include_directories(lvbase
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_definitions(lvbase PRIVATE LV_BASE_LIB)
target_compile_features(lvbase PRIVATE cxx_std_17)

set(ENABLE_STACK_TRACE OFF)

if(DEBUG_BUILD)
    set(ENABLE_STACK_TRACE ON)
endif()

if(ENABLE_STACK_TRACE)
    if (WIN32)
        target_compile_definitions(lvbase PUBLIC USE_STACK_TRACE)
        target_link_libraries(lvbase dbghelp)
    elseif(UNIX AND NOT APPLE)
        if ( BUILD_UNIX_BFD_TRACE )
            target_compile_definitions(lvbase PRIVATE ENABLE_STACK_TRACE)
            target_link_libraries(lvbase -lbfd -ldl)
        endif()
    endif()
endif()

# Setup rpaths

if(APPLE)
    set_target_properties( lvbase PROPERTIES MACOSX_RPATH TRUE )
endif()


# Include 3rdparty library - date

target_include_directories(lvbase
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/date/include"
)

# Include 3rdparty library - libb64

target_include_directories(lvbase
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libb64/include"
)
target_sources(lvbase
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libb64/src/cdecode.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libb64/src/cencode.c"
)

# Include 3rdparty library - backward

if(UNIX)
    target_include_directories(lvbase
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/backward"
    )
endif()

# Include 3rdparty library - rapidjson

target_include_directories(lvbase
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/rapidjson/include"
)

# Include 3rdparty library - utf8proc

target_compile_definitions(lvbase PRIVATE UTF8PROC_EXPORTS)
target_include_directories(lvbase
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/utf8proc"
)
target_sources(lvbase
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/utf8proc/utf8proc.c"
)

# Link experimental filesystem on older compilers

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.4)
        message(FATAL_ERROR "GCC version must be newer than 5.4.")
    elseif (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
        message(WARNING "Older Verison of GCC detected (<8.0). Using Legacy C++ support")
        target_link_libraries(lvbase -lstdc++fs)
    endif()
endif()

if (WIN32)
    target_sources(lvbase
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/src/stacktrace_win.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/library_win.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/libraryloadpath_win.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/applicationcontext_win.cpp"
    )
endif ()

if(UNIX AND NOT APPLE)
    target_sources(lvbase
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/src/stacktrace_unix.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/library_unix.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/libraryloadpath_unix.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/applicationcontext_unix.cpp"
    )
endif()

if(APPLE)
    target_sources(lvbase
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/src/stacktrace_unix.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/library_unix.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/libraryloadpath_unix.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/applicationcontext_mac.cpp"
    )
endif()

if(BUILD_TESTS)
    add_subdirectory(test)
endif()
