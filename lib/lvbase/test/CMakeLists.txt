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


if (WIN32)
    set(FILE_TO_ADD "${LIBRARY_DEPLOY_PATH}/lvbase.dll")
    set(FILE_TO_ADD_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}")

    add_custom_command(
        OUTPUT ${FILE_TO_ADD_OUTPUT}
        DEPENDS ${FILE_TO_ADD}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FILE_TO_ADD} "${FILE_TO_ADD_OUTPUT}"
    )
    list(APPEND ADD_DEPENDENCIES_OUTPUT "${FILE_TO_ADD_OUTPUT}")

    add_custom_target(LVBASE_TEST_COPY ALL DEPENDS ${ADD_DEPENDENCIES_OUTPUT})
endif()




