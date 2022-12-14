enable_testing(true)

add_executable(lvviewtest)
add_test(NAME lvviewtest COMMAND lvviewtest)

target_sources(lvviewtest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/enginetest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mlnodetoqmltest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/visuallogqttest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/linecapturetest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memorytest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lvviewstubs/engineteststub.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lvviewstubs/visuallogteststub.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lvviewstubs/shareddatateststub.cpp"
)

target_include_directories(lvviewtest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/lvviewstubs")
target_link_libraries(lvviewtest PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Test
    lvbase
    lvview
)


