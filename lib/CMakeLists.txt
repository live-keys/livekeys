
add_subdirectory(lvbase)
add_subdirectory(lvview)
add_subdirectory(lveditor)
add_subdirectory(lveditqmljs)

if(SHOULD_BUILD_ELEMENTS)
    add_subdirectory(lvelements)
endif()


