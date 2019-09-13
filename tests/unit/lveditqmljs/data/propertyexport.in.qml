/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

Item {
    id: root
    property int intProperty
    property string stringProperty
    property Rectangle rectangleProperty

    signal exportSignal(string stringArgument, int intArgument)

    Rectangle{
        property int parentIntProperty

        Item{
            id: scope

            property var scopeVarProperty

            width : parent.width
            onWidthChanged : {
                console.log("width changed")
            }
        }
    }
}
