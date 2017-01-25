/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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
import QtQuick.Controls.Styles 1.2

Rectangle{
    id : root
    width: 500
    height : contentHeight + 5 > 200 ? 200 : contentHeight + 5
    color : backgroundColor
    
    property int contentHeight : rootScroll.flickableItem.contentHeight
    
    property var configurationData : ({
        'sample1' : 10
    })

    property list<Component> configurationFields : [
        Component{
            ConfigurationField{
                label : "Sample1"
                editor : InputBox{
                    text: root.configurationData["sample1"];
                    onTextChanged: root.configurationData["sample1"] = text;
                }
            }
        }
    ]

    property color backgroundColor : "#081520"

    ScrollView{
        id : rootScroll
        
        style : LiveCVScrollStyle{}
        anchors.fill: parent

        Column{
            width: parent.width

            Repeater{
                id : configurationView
                model: root.configurationFields ? root.configurationFields : []
                delegate : Component{
                    Rectangle{
                        width: 300
                        height: 30
                        color : root.backgroundColor

                        Loader{
                            id: fieldLoader
                            sourceComponent: modelData
                        }

                    }
                }
            }
        }

    }

}
