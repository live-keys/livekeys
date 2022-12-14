if (WIN32)
    if(DEFINED ENV{V8_DIR})
        set(V8_INCLUDE_DIRS $ENV{V8_DIR}/include)
        set(V8_LIBRARY_DIR $ENV{V8_DIR}/lib/Release)
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(V8_LIBRARY_DIR $ENV{V8_DIR}/lib/Debug)
        endif()

        set(V8_LIBRARY_NAMES v8 v8_libplatform)

        FOREACH(LIB_NAME ${V8_LIB_NAMES})
            FIND_LIBRARY(FOUND_LIB_${LIB_NAME} ${LIB_NAME} PATHS ${V8_LIBRARY_DIR})

            IF(NOT FOUND_LIB_${LIB_NAME})
                MESSAGE(FATAL_ERROR "LIBRARY: '${LIB_NAME}' NOT FOUND")
            ENDIF()

            LIST(APPEND V8_LIBS FOUND_LIB_${LIB_NAME})

            file(COPY ${V8_LIBRARY_DIR}/${LIB_NAME}.dll DESTINATION ${DEPLOY_PATH})

        ENDFOREACH(LIB_NAME)

        set(V8_FOUND TRUE)
    endif()
endif()

if(APPLE)
    execute_process(COMMAND which d8 OUTPUT_VARIABLE V8_OUTPUT RESULT_VARIABLE V8_RESULT)
    if(NOT V8_RESULT EQUAL 0)
        execute_process(COMMAND which v8 OUTPUT_VARIABLE V8_OUTPUT RESULT_VARIABLE V8_RESULT)
    endif()

    if(NOT V8_RESULT EQUAL 0)
        execute_process(COMMAND which /usr/local/bin/d8 OUTPUT_VARIABLE V8_OUTPUT RESULT_VARIABLE V8_RESULT)
    endif()
    if(NOT V8_RESULT EQUAL 0)
        execute_process(COMMAND which /usr/local/bin/v8 OUTPUT_VARIABLE V8_OUTPUT RESULT_VARIABLE V8_RESULT)
    endif()

    if(NOT V8_RESULT EQUAL 0)
        execute_process(COMMAND which /usr/bin/d8 OUTPUT_VARIABLE V8_OUTPUT RESULT_VARIABLE V8_RESULT)
    endif()
    if(NOT V8_RESULT EQUAL 0)
        execute_process(COMMAND which /usr/bin/v8 OUTPUT_VARIABLE V8_OUTPUT RESULT_VARIABLE V8_RESULT)
    endif()

    if(NOT V8_RESULT EQUAL 0)
        MESSAGE(WARNING "Failed to find V8 installation.")
    else()
        get_filename_component(V8_DIR  ${V8_OUTPUT} DIRECTORY)
        get_filename_component(V8_NAME ${V8_OUTPUT} NAME)

        execute_process(COMMAND readlink d8 WORKING_DIRECTORY ${V8_DIR} OUTPUT_VARIABLE V8_SYMFOLLOW RESULT_VARIABLE V8_RESULT)
        get_filename_component(V8_PATH ${V8_SYMFOLLOW} REALPATH BASE_DIR ${V8_DIR})
        get_filename_component(V8_BIN_DIR ${V8_PATH} DIRECTORY)
        get_filename_component(V8_DIR ${V8_BIN_DIR} DIRECTORY)

        set(V8_INCLUDE_DIRS ${V8_DIR}/include)
        set(V8_LIBRARY_DIR ${V8_DIR}/lib/Release)

        set(V8_LIB_NAMES v8 v8_libplatform)

        FIND_LIBRARY(FOUND_LIB_V8 v8 PATHS ${V8_LIBRARY_DIR})
        IF(NOT FOUND_LIB_V8)
            MESSAGE(FATAL_ERROR "LIBRARY: '${FOUND_LIB_V8}' NOT FOUND")
        ENDIF()
        LIST(APPEND V8_LIBS ${FOUND_LIB_V8})

        FIND_LIBRARY(FOUND_LIB_V8PLATFORM v8_libplatform PATHS ${V8_LIBRARY_DIR})
        IF(NOT FOUND_LIB_V8PLATFORM)
            MESSAGE(FATAL_ERROR "LIBRARY: '${FOUND_LIB_V8PLATFORM}' NOT FOUND")
        ENDIF()
        LIST(APPEND V8_LIBS ${FOUND_LIB_V8PLATFORM})

        set(V8_FOUND TRUE)

    endif()
endif()

if(UNIX AND NOT APPLE)
    find_package(V8)
endif()
