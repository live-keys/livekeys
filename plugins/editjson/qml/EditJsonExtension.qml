import QtQuick 2.3
import editor 1.0
import editjson 1.0 as Ej

WorkspaceExtension{
    id: root

    property var settings: {
        return {
            "style" : {
                "text" : "#fff",
                "number" : "#ba761d",
                "string" : "#86a930",
                "operator" : "#86a930",
                "constant.builtin": "#93672f"
            }
        }
    }

    interceptLanguage : function(document, handler, ext){
        var extLower = ext.toLowerCase()

        if ( extLower === 'json' ){
            var handler = Ej.EditJsonObject.createHandler(
                document, handler, "json", root.settings
            )
            handler.setLanguage(Ej.EditJsonObject.language)
            handler.createHighlighter(
                "(string) @string \n" +
                "(number) @number \n" +
                "(true) @constant.builtin \n" +
                "(false) @constant.builtin \n" +
                "(null) @constant.builtin \n",
                root.settings["style"]
            )
            return handler
        }
        return null
    }
    commands: { return {} }
    objectName : "editjson"
}
