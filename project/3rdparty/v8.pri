# Avoid multiple inclusion
isEmpty(V8CONFIG_PRO){
V8CONFIG_PRO = 1

win32{
    # Windows Configuration

    # Windows Configuration assumes the following structure;
    #
    #  V8_DIR - environment variable to be set
    #  V8_DIR/lib/Debug - path to debug libraries, dlls and snaposhot files
    #  V8_DIR/lib/Release - path to release libraries, dlls and snaposhot files
    #  V8_DIR/include - path to include files

    V8_DIR = $$(V8_DIR)

    INCLUDEPATH += $$(V8_DIR)/include

    V8_LIBRARY_PATH = $$(V8_DIR)/lib/Release
    CONFIG(debug, debug|release):V8_LIBRARY_PATH = $$(V8_DIR)/lib/Debug

    LIBS += -L$$V8_LIBRARY_PATH -lv8_libplatform
    LIBS += -L$$V8_LIBRARY_PATH -lv8


    defineTest(copyV8) {
        files = $$1

        for(FILE, files) {
            DDIR = $$2

            # Replace slashes in paths with backslashes
            win32:FILE ~= s,/,\\,g
            win32:DDIR ~= s,/,\\,g

            QMAKE_POST_LINK += $$QMAKE_COPY \"$$FILE\" \"$$DDIR\" $$escape_expand(\\n\\t)
        }
        export(QMAKE_POST_LINK)
    }

    defineTest(mkV8Dir){
        DDIR = $$1
        !exists($$DDIR){
            win32:DDIR ~= s,/,\\,g
            QMAKE_POST_LINK += $(CHK_DIR_EXISTS) \"$${DDIR}\" $(MKDIR) \"$${DDIR}\" $$escape_expand(\n\t)
            export(QMAKE_POST_LINK)
        }
    }

    defineTest(deployV8){
        mkV8Dir($$DEPLOY_PATH/external)
        mkV8Dir($$DEPLOY_PATH/external/v8)
        copyV8($${V8_LIBRARY_PATH}/icui18n.dll, $$DEPLOY_PATH)
        copyV8($${V8_LIBRARY_PATH}/icuuc.dll, $$DEPLOY_PATH)
        copyV8($${V8_LIBRARY_PATH}/v8.dll, $$DEPLOY_PATH)
        copyV8($${V8_LIBRARY_PATH}/v8_libbase.dll, $$DEPLOY_PATH)
        copyV8($${V8_LIBRARY_PATH}/v8_libplatform.dll, $$DEPLOY_PATH)

        copyV8($${V8_LIBRARY_PATH}/natives_blob.bin, $${DEPLOY_PATH}/external/v8)
        copyV8($${V8_LIBRARY_PATH}/snapshot_blob.bin, $${DEPLOY_PATH}/external/v8)
    }

}
else:macx{
    # Mac OS configuration

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

    defineTest(deployV8){}
}
else:unix{
    defineTest(deployV8){}
}

}
