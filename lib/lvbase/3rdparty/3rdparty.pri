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
