import QtQuick 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2
import QtQuick.Window 2.0

import base 1.0

ApplicationWindow{
    id : root

    visible: true
    width: 1240
    minimumWidth: 640
    height: 700
    minimumHeight: 400
    color : "#293039"
    objectName: "window"

    QtObject{
        id: dialogs
    }
}
