
add_library(lvelementsengine SHARED)

set_target_properties(lvelementsengine PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_DEPLOY_PATH}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBRARY_DEPLOY_PATH}"
)

target_include_directories(lvelementsengine
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions(lvelementsengine PRIVATE LV_ELEMENTS_ENGINE_LIB)

target_link_libraries(lvelementsengine PRIVATE lvbase lvelementscompiler)

target_compile_definitions(lvelementsengine PRIVATE BUILD_ELEMENTS_ENGINE)
target_include_directories(lvelementsengine PRIVATE ${V8_INCLUDE_DIRS})
target_link_libraries(lvelementsengine PRIVATE ${V8_LIBS})

target_sources(lvelementsengine PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/bindingpath.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/function.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/jsmodule.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/languagescanner.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/metaobject.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/metaobjecttypeinfo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/modulefilejsdata.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/value.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/element.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/property.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/event.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/eventfunction.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/context.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/script.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/component.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/constructor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/callable.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/object.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/list.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/buffer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/container.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/errorhandler.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mlnodetojs.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/visuallogjsobject.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/modulelibrary.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tuple.cpp"
)

