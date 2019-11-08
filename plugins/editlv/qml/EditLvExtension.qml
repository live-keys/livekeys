import QtQuick 2.3
import editor 1.0
import editlv 1.0

LiveExtension{
    id: root

    globals : LanguageLvExtension{}
    interceptLanguage : function(document, handler, ext){
        var extLower = ext.toLowerCase()

        if ( extLower === 'lv' ){
            return globals.createHandler(document, handler)
        }
        return null
    }
    commands: { return {} }

    objectName : "editlv"
}
