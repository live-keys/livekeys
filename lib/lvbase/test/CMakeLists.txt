enable_testing(true)

add_executable(lvbasetest)
add_test(NAME lvbasetest COMMAND lvbasetest)

if(CATCH2_AMALGAMATED_PATH)
    target_compile_definitions(lvbasetest PRIVATE CATCH_AMALGAMATED_CUSTOM_MAIN)
    target_include_directories(lvbasetest PRIVATE "${CATCH2_AMALGAMATED_PATH}")
    target_sources(lvbasetest PRIVATE "${CATCH2_AMALGAMATED_PATH}/catch_amalgamated.cpp")
    target_compile_definitions(lvbasetest PRIVATE CATCH2_AMALGAMATED)
endif()

target_include_directories(lvbasetest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_sources(lvbasetest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/datetimetest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/commandlineparsertest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bytebuffertest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mlnodetest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mlnodetojsontest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/filesystemtest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/visuallogtest.cpp"
)

target_link_libraries(lvbasetest PRIVATE lvbase)

if(BUILD_LVBASE_STATIC)
    target_compile_definitions(lvbasetest PRIVATE LV_BASE_STATIC)
endif()


if(MSVC)
    set_target_properties(lvbasetest PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/$<0:>) 
endif()

if (WIN32 AND (NOT BUILD_LVBASE_STATIC))
    copy_file_list(LVBASE_TEST_COPY "${LIBRARY_DEPLOY_PATH}/lvbase.dll" ${CMAKE_CURRENT_BINARY_DIR})
endif()




