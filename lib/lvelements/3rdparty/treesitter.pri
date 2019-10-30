HEADERS += \
    $$PWD/treesitter/lib/include/tree_sitter/*.h \

SOURCES += \
    $$PWD/treesitter/lib/src/lib.c

INCLUDEPATH += \
    $$PWD/treesitter/lib/src \
    $$PWD/treesitter/lib/include \
    $$PWD/../utf8proc
