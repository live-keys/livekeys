# Add backward

unix{
    INCLUDEPATH += $$PWD/backward
    HEADERS += $$PWD/backward/backward.hpp
}

# Add rapidjson

INCLUDEPATH += $$PWD/rapidjson/include

# Add utf8proc

DEFINES += UTF8PROC_EXPORTS
INCLUDEPATH += $$PWD/utf8proc

HEADERS += $$PWD/utf8proc/utf8proc.h
SOURCES += $$PWD/utf8proc/utf8proc.c

# Add date

INCLUDEPATH += $$PWD/date/include

# Add libb64

INCLUDEPATH += $$PWD/libb64/include
HEADERS += \
    $$PWD/libb64/include/b64/ccommon.h \
    $$PWD/libb64/include/b64/cdecode.h \
    $$PWD/libb64/include/b64/cencode.h \
    $$PWD/libb64/include/b64/decode.h \
    $$PWD/libb64/include/b64/encode.h \

SOURCES += \
    $$PWD/libb64/src/cdecode.c \
    $$PWD/libb64/src/cencode.c
