add_subdirectory(lvviewtest)
add_subdirectory(lveditqmljs)
add_subdirectory(lveditortest)

if(SHOULD_BUILD_ELEMENTS_ENGINE AND BUILD_ELEMENTS_COMPILER)
    add_subdirectory(lvelementstest)
endif()
