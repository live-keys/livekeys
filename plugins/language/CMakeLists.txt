
add_library(language SHARED)

set_target_properties(language PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${MODULE_DEPLOY_PATH}/language"
)

if(APPLE)
    set_target_properties(language PROPERTIES LINK_FLAGS
        "-Wl,-rpath,@executable_path/../Link -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@executable_path/../Frameworks/Live.framework/Libraries")
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(language PROPERTIES LINK_FLAGS
        "-Wl,-rpath,$$ORIGIN/../../link"
    )
endif()


add_output_files(language_lv        ${CMAKE_CURRENT_SOURCE_DIR}/lv        ${MODULE_DEPLOY_PATH}/language          language_lv_sources)

target_sources(language PRIVATE ${language_lv_sources})

target_sources(language PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/languagemodule.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/languagescannerwrap.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/moduleinfocapture.cpp"
)

target_include_directories(language
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_compile_definitions(language PRIVATE LV_LANGUAGE_MODULE)

target_link_libraries(language
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick )

target_link_libraries(language PRIVATE lvbase lvview lvelementscompiler lvelementsengine)
