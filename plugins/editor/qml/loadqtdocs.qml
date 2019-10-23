import QtQuick 2.3
import QtWebEngine 1.7
import editor 1.0
import fs 1.0 as Fs

DocumentationLoader{
    id: root

    load: function(type, packageName, path){

        var docUrl = lk.layers.workspace.docsPath() + '/qt'
        var docPage = pageFactory.createObject()

        var hashIndex = path.indexOf('#')
        var htmlPath = ''
        var htmlHash = ''

        if ( hashIndex >= 0 ){
            var fileUrl = path.substring(0, hashIndex)
            var itemUrl = path.substring(hashIndex + 1)

            var qmlType = ''
            var qmlTypeProperty = ''
            var qmlTypeNameSpace = fileUrl

            var typeSplitIndex = itemUrl.indexOf('.')

            if ( typeSplitIndex > -1 ){
                qmlType = itemUrl.substring(0, typeSplitIndex)
                qmlTypeProperty = itemUrl.substring(typeSplitIndex + 1)
            } else {
                qmlType = itemUrl
            }

            htmlPath = qmlTypeNameSpace.replace('.', '') + '/qml-' +
                        qmlTypeNameSpace.replace('.', '-') + '-' +
                        qmlType + '.html'
            htmlHash = qmlTypeProperty.length > 0 ? '#' + qmlTypeProperty + '-prop' : ''
        } else {
            htmlPath = path.replace('.', '') + '/' +
                    path.replace('.', '-') + '-qmlmodule.html'
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
