enable_testing(true)

add_executable(lveditqmljstest)
add_test(NAME lveditqmljstest COMMAND lveditqmljstest)

add_output_files(lveditqmljstest_data ${CMAKE_CURRENT_SOURCE_DIR}/data  ${TESTS_DEPLOY_PATH}/unit/lveditqmljstest lveditqmljstest_data_sources)

target_sources(lveditqmljstest PRIVATE ${lveditqmljstest_data_sources})

target_sources(lveditqmljstest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qtestrunner.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qtypestub.cpp"
)

target_include_directories(lveditqmljstest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(lveditqmljstest PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Test
    lvbase
    lvview
    lveditor
    lveditqmljs
)


