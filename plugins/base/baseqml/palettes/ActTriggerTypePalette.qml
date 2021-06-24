/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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
import live 1.0
import base 1.0
import visual.input 1.0 as Input

CodePalette{
    id: palette

    type: "qml/base#Act.trigger"

    property QtObject theme: lk.layers.workspace.themes.current

    item: Input.DropDown {
        id: dropdown
        width: 150
        model: ["PropertyChange", "Manual"]
        property var values: [
            Act.PropertyChange,
            Act.Manual
        ]

        function baseQualifier(){
            var imports = editFragment.codeHandler.importsModel()
            if (!imports)
                return ''

            var baseImport = imports.getImportAtUri('base')
            if (!baseImport)
                return ''

            return baseImport.as.length ? baseImport.as : ''
        }

        onCurrentIndexChanged: {
            if ( isBindingChange() )
                return
            if (!editFragment)
                return

            var type = values[dropdown.currentIndex]
            var qual = baseQualifier()
            palette.value = type

            if (qual.length){
                var writeValue = qual + ".Act." + model[dropdown.currentIndex]
                editFragment.write({
                    "__ref": writeValue
                })
            } else {
                editFragment.write({
                    '__ref' : "Act." + model[dropdown.currentIndex]
                })
            }
        }
    }

    onValueFromBindingChanged: {
        if ( value === Act.Manual ){
            dropdown.currentIndex = 1
        } else {
            dropdown.currentIndex = 0
        }
    }
    onInit: {
        if ( value === Act.Manual ){
            dropdown.currentIndex = 1
        } else {
            dropdown.currentIndex = 0
        }
    }

    onEditFragmentChanged: {
        editFragment.whenBinding = function(){}
    }
}
