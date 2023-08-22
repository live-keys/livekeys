

add_subdirectory(compiler)

if(SHOULD_BUILD_ELEMENTS_ENGINE)
    target_compile_definitions(lvelementscompiler PRIVATE BUILD_ELEMENTS_ENGINE)
    add_subdirectory(engine)
endif()
