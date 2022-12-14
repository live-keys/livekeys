add_subdirectory(lvbasetest)
add_subdirectory(lvviewtest)
add_subdirectory(lveditqmljs)
add_subdirectory(lveditortest)

if(SHOULD_BUILD_ELEMENTS)
    add_subdirectory(lvelementstest)
endif()
