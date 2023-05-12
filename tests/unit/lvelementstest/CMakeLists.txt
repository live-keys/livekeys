enable_testing(true)

add_executable(lvelementstest)
add_test(NAME lvelementstest COMMAND lvelementstest)

add_output_files(lvelementstest_data ${CMAKE_CURRENT_SOURCE_DIR}/data  ${TESTS_DEPLOY_PATH}/unit/lvelementstest/data lvelementstest_data_sources)

target_sources(lvelementstest PRIVATE ${lvelementstest_data_sources})

target_sources(lvelementstest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/jsobjecttest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/metaobjectfunctiontest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/eventtest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/jseventtest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/jspropertytest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/jserrorhandlingtest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/jsfunctiontest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/jsmethodtest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/jsmemorytest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/jstypestest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/jslisttest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lvcompiletest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mlnodetojstest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/testpack.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/jstupletest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/jsinheritancetest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lvparsetest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/metaobjecttypeinfotest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lvimportstest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lvelparseddocumenttest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lvellanguageinfoserializationtest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lvparseerrortest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lvelementsstubs/pointstub.cpp"
)

target_include_directories(lvelementstest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(lvelementstest PRIVATE
    Qt${QT_VERSION_MAJOR}::Test
    lvbase
    lvelements
)


if (WIN32)
    copy_file_list(LVELEMENTS_TEST_COPY "${LIBRARY_DEPLOY_PATH}/lvbase.dll;${LIBRARY_DEPLOY_PATH}/lvelements.dll" ${CMAKE_CURRENT_BINARY_DIR})
endif()
