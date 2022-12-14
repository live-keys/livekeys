enable_testing(true)

add_executable(lveditortest)
add_test(NAME lveditortest COMMAND lveditortest)

target_sources(lveditortest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lvlinecontroltest.cpp"
)

target_include_directories(lveditortest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(lveditortest PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Test
    lvbase
    lvview
    lveditor
)


