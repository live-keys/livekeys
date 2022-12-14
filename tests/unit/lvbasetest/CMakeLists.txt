enable_testing(true)

add_executable(lvbasetest)
add_test(NAME lvbasetest COMMAND lvbasetest)

target_sources(lvbasetest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/commandlineparsertest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mlnodetest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mlnodetojsontest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/filesystemtest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/datetimetest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bytebuffertest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/visuallogtest.cpp"
)

target_include_directories(lvbasetest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(lvbasetest PRIVATE Qt${QT_VERSION_MAJOR}::Test lvbase)


