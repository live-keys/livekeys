import QtQuick 2.3
import QtWebEngine 1.7
import editor 1.0
import fs 1.0 as Fs

DocumentationLoader{
    id: root

    load: function(type, packageName, path){
        var docUrl = lk.layers.workspace.docsPath()
        var docPage = pageFactory.createObject()

        var hashIndex = path.indexOf('#')
        var htmlPath = ''
        var htmlHash = ''

        if ( hashIndex >= 0 ){
            var fileUrl = path.substring(0, hashIndex)
            var hashUrl = path.substring(hashIndex + 1)

            htmlPath = 'plugin_' + fileUrl.replace('.', '_') + '.html'
            htmlHash = '#' + hashUrl.replace('.', '-')
        } else {
            htmlPath = 'plugin_' + path.replace('.', '_') + '.html'
        }

        var filePath = docUrl + '/' + htmlPath
        var url = filePath + htmlHash

        docPage.loadDocumentationHtml(url)

        return docPage
    }

    property var pageFactory : Component{
        DocumentationView{}
    }
}
