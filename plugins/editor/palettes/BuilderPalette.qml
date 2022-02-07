/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import editor 1.0
import editqml 1.0 as QmlEdit
import live 1.0
import visual.input 1.0 as Input

CodePalette{
    id: palette
    type : "qml/Component"

    property QtObject theme: lk.layers.workspace.themes.current

    function createPath(){
        if ( pathInput.path.startsWith('%project%') ){
            return 'project.path("' + pathInput.path.substr(10) + '")'
        }
        return '"' + pathInput.path + '"'
    }

    function resolvePath(){
        if ( pathInput.path.startsWith('%project%') ){
            return lk.layers.workspace.project.path(pathInput.path.substr(10))
        }
        return pathInput.path
    }

    function assignBuilder(){
        if ( hasBuilder ){

            var builderComponentCode = 'import editqml 1.0\n\nBuilder{ '
            builderComponentCode += 'id: ' + builderId + '; '
            builderComponentCode += 'source: "' + resolvePath() + '"'
            builderComponentCode += ' }'

            var comp = engine.createComponent(builderComponentCode, "Builder.qml")
            palette.value = comp

            var imports = editFragment.language.importsModel()
            if ( imports ){
                var editQmlImport = imports.getImportAtUri('editqml')
                if ( editQmlImport ){
                    var builderCode =  (editQmlImport.as.length ? editQmlImport.as + '.' : '') + 'Builder{ '
                    builderCode += 'id: ' + builderId + '; '
                    builderCode += 'source: ' + createPath()
                    builderCode += ' }'
                    editFragment.write({'__ref': builderCode })
                } else {
                    lk.layers.workspace.messages.pushWarning("\'editqml\' import has not been added to this document. Add 'import editqml 1.0' to add a builder.", 100)
                }
            }
        } else {
            palette.value = null
            editFragment.write({'__ref': editFragment.defaultValue() })
        }
    }

    function createBuilder(){
        hasBuilder = true
        builderId = 'builder'
    }

    property bool hasBuilder: false
    property string builderId: ''
    property var builderPath: { return {} }
    property string builderPathString: {
        return (builderPath.from === 'project' ? '%project%/' : '') + (builderPath.path ? builderPath.path : '')
    }

    item: Item{

        width: 280
        height: 50

        Input.TextButton{
            visible: !hasBuilder
            text: 'Add Builder'
            height: 22
            width: 70
            style: palette.theme.formButtonStyle
            onClicked : palette.createBuilder()
        }

        Input.Label{
            id: title
            anchors.top: parent.top
            anchors.topMargin: 2
            anchors.left: parent.left
            anchors.leftMargin: 5
            height: 20
            width: parent.width
            textStyle: palette.theme.inputLabelStyle.textStyle

            text: '#' + palette.builderId
            visible: hasBuilder
        }

        Input.PathInputBox{
            id: pathInput
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.top: parent.top
            anchors.topMargin: 25
            width: parent.width - 35
            height: 25
            visible: hasBuilder
            style: QtObject{
                property QtObject inputBoxStyle: palette.theme.inputStyle
                property QtObject buttonStyle: palette.theme.formButtonStyle
            }
        }

        Input.Button{
            anchors.top: parent.top
            anchors.topMargin: 25
            anchors.right: parent.right
            width: 30
            height: 25
            content: palette.theme.buttons.apply
            visible: hasBuilder
            onClicked: palette.assignBuilder()
        }
    }

    onInit: {
        if ( editFragment.objectInitializerType() === "qml/editqml#Builder"){
            var contents = editFragment.readValueText()
            var declarations = QmlEdit.Syntax.parseObjectDeclarations(contents)
            if ( 'id' in declarations.properties ){
                var builderId = contents.substr(declarations.properties.id.begin, declarations.properties.id.propertyEnd - declarations.properties.id.begin)
                var builderPath = {}
                if ( 'source' in declarations.properties ){
                    var builderSource = contents.substr(declarations.properties.source.valueBegin, declarations.properties.source.end - declarations.properties.source.valueBegin)
                    builderPath = QmlEdit.Syntax.parsePathDeclaration(builderSource)
                }

                if ( builderId.length ){
                    palette.hasBuilder = true
                    palette.builderId = builderId
                    palette.builderPath = builderPath
                    pathInput.path = palette.builderPathString
                }
            }
        }
    }
}
