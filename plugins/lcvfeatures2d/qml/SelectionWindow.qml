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
import QtQuick.Window 2.2
import live 1.0
import lcvcore 1.0

Window{
    id : selectionWindow

    color: "#05111b"

    width : matView.width
    height : matView.height + 30

    property alias mat : matView.mat
    signal regionSelected(Mat region, int x, int y, int width, int height)

    MatView{
        id : matView

        SelectionArea{
            id : selectionArea
            anchors.fill: parent
            onSelected : {
                selectedRegion.regionX = x
                selectedRegion.regionY = y
                selectedRegion.regionWidth = width
                selectedRegion.regionHeight = height
            }
        }
    }

    MatRoi{
        id: selectedRegion
        input : matView.mat
        visible : false
        regionX : 0
        regionY : 0
        regionWidth : selectionArea.width
        regionHeight : selectionArea.height
    }
    
    TextButton{
        width : 100
        height : 30
        text : "Done"
        anchors.bottom: parent.bottom
        onClicked : {
            selectionWindow.regionSelected(
                selectedRegion.output.createOwnedObject(),
                selectedRegion.regionX,
                selectedRegion.regionY,
                selectedRegion.regionWidth,
                selectedRegion.regionHeight
            )
            selectionWindow.close()
        }
    }
}
