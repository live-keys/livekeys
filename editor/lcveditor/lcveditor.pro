
LIB_NAME = lcveditor
include($$getConfigFile(is_editor_lib.pri))

DEFINES += Q_LCVEDITOR_LIB

include($$PWD/src/lcveditor.pri)
