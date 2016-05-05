/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
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

pragma Singleton
import QtQuick 2.3

QtObject{
    property color background: "#05111b"
    property color editorBackground : "#071723"

    property color textColor : "#ffffff"
    property int textPixelSize : 14
    property string textFont : "Ubuntu Mono, Courier New, Courier"

    property color headerColor : "#061a29"
    property color elementBackgroundColor : "#081520"
    property color elementHighlightBackgroundColor : "#071825"

    property color buttonColor : "#061a29"
    property color buttonHighlightColor : "#082134"
    property color darkElementBackgroundColor : "#020d14"
    property color darkElementBorderColor : "#031728"

    property color inputBoxColor : "#050e15"
    property color inputboxBorderColor : "#031728"
}
