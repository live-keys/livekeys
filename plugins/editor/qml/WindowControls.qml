import QtQuick 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2

QtObject{
    property FileDialog saveFileDialog: null
    property FileDialog openFileDialog: null
    property FileDialog openDirDialog: null
    property var messageDialog: null
    property QtObject runSpace: null
    property Item editSpace: null
    property Timer createTimer: null
//    property Item paletteBox: null
    property Component paletteBoxFactory: null
}
