
win32{

}
else:macx{

    # Find out where d8 or v8 resides
    # Assume that v8 and d8 are in /bin dir, where the libraries are,
    # and the include files are in /include dir.

    V8_PATH = $$system("readlink `which d8`")
    V8_DIR  = $$system("dirname `which d8`")
    isEmpty(V8_PATH){
        V8_PATH = $$system("readlink `which v8`")
        V8_DIR  = $$system("dirname `which v8`")
    }
    isEmpty(V8_PATH){
        error("Failed to find v8 installation. Make sure v8 has been installed and can can be located")
    }

    V8_ABSOLUTE_PATH = $$absolute_path($$V8_PATH, $$V8_DIR)

    V8_LIBRARY_PATH = $$dirname(V8_ABSOLUTE_PATH)
    V8_INCLUDE_PATH = $$dirname(V8_LIBRARY_PATH)/include

    INCLUDEPATH += $$V8_INCLUDE_PATH
    LIBS += -L$$V8_LIBRARY_PATH -lv8_libplatform
    LIBS += -L$$V8_LIBRARY_PATH -lv8
}
else:unix{

}
