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
import lcvcore 1.0

CodePalette{
    id: palette

    type : "qml/ImageFile"

    item: Item{
        id: imageFileBox
        width: 280
        height: 30

        property ImageFile imageFile : null

        PathInputBox{
            id: inputBox
            anchors.fill: parent

            onPathSelected: {
                imageFileBox.imageFile.source = path
                if ( !palette.isBindingChange() ){
                    extension.writeProperties({
                        'source' : imageFileBox.imageFile.source
                    })
                }
            }
        }

    }

    onInit: {
        imageFileBox.imageFile = value
    }

    onExtensionChanged: {
        extension.whenBinding = function(){
            extension.writeProperties({
                'source' : palette.value.source
            })
        }
    }
}
