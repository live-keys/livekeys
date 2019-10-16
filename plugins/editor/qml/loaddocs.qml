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
        var url = "file://" + filePath + htmlHash

        var f = Fs.File.open(filePath, Fs.File.ReadOnly)
        if (f){
            var contents = f.readAll().toString()

            var styledContents = contents.replace('</head>',
                '<style>\n' +
                    '#main{padding: 0px;}\n' +
                    '#wrapper{padding: 15px;max-width: 100%;flex: 0 0 100%;background-color: #03090d;}\n' +
                    '#indexList{display: none;}\n' +
                '</style></head>\n')

            docPage.loadHtml(styledContents, url)
        }

        return docPage
    }

    property string styleSourceCode : ''

    property var pageFactory : Component{

        WebEngineView{
            anchors.fill: parent ? parent : undefined

            property WebEngineScript userScript : WebEngineScript{
                name: "indexTableRemoval"
                sourceCode: root.styleSourceCode
            }
            userScripts: [userScript]
        }
    }
}
