enable_testing(true)

add_executable(lvelementscompilertest)
add_test(NAME lvbasetest COMMAND lvbasetest)

add_output_files(lvelementscompilertest_data ${CMAKE_CURRENT_SOURCE_DIR}/data  ${CMAKE_CURRENT_BINARY_DIR}/data lvelementscompilertest_data_sources)
target_sources(lvelementscompilertest PRIVATE ${lvelementscompilertest_data_sources})

if(CATCH2_AMALGAMATED_PATH)
    target_compile_definitions(lvelementscompilertest PRIVATE CATCH_AMALGAMATED_CUSTOM_MAIN)
    target_include_directories(lvelementscompilertest PRIVATE "${CATCH2_AMALGAMATED_PATH}")
    target_sources(lvelementscompilertest PRIVATE "${CATCH2_AMALGAMATED_PATH}/catch_amalgamated.cpp")
    target_compile_definitions(lvelementscompilertest PRIVATE CATCH2_AMALGAMATED)
endif()

target_include_directories(lvelementscompilertest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_sources(lvelementscompilertest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/parsetest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/parseerrortest.cpp"
)

target_link_libraries(lvelementscompilertest PRIVATE lvbase lvelementscompiler)

if(MSVC)
    set_target_properties(lvelementscompilertest PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/$<0:>) 
endif()


if(BUILD_LVBASE_STATIC)
    target_compile_definitions(lvelementscompilertest PRIVATE LV_BASE_STATIC)
endif()
if(BUILD_LVELEMENTSCOMPILER_STATIC)
    target_compile_definitions(lvelementscompilertest PRIVATE LV_ELEMENTS_COMPILER_STATIC)
endif()

if (WIN32 AND (NOT BUILD_LVBASE_STATIC) AND (NOT BUILD_LVELEMENTSCOMPILER_STATIC))
    copy_file_list(LVELEMENTSCOMPILER_TEST_COPY "${LIBRARY_DEPLOY_PATH}/lvbase.dll;${LIBRARY_DEPLOY_PATH}/lvelementscompiler.dll" ${CMAKE_CURRENT_BINARY_DIR})
endif()




