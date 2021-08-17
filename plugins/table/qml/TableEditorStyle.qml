import QtQuick 2.3
import visual.input 1.0

QtObject{
    property color iconColor: 'black'

    property color cellBackgroundColor: '#fff'
    property color cellBorderColor: '#aaa'
    property double cellBorderSize: 1
    property QtObject cellTextStyle: TextStyle{ color: 'black' }

    property color selectedCellBorderColor: '#333'
    property color selectedCellBackgroundColor: '#aaa'
    property QtObject selectedCellTextStyle: TextStyle{
        color: 'black'
    }

    property QtObject cellInputStyle: InputBoxStyle{
        backgroundColor: '#aaa'
    }

    property color headerCellBackgroundColor: '#ccc'
    property color headerCellBorderColor: '#aaa'
    property double headerCellBorderSize: 2
    property QtObject headerCellTextStyle: TextStyle{
        color: 'black'
    }

    property double defaultCellWidth: 90
    property double defaultCellHeight: 25

    property double minimumCellWidth: 40
    property double minimumCellHeight: 15

    property double headerCellWidth: 50
    property double headerCellHeight: defaultCellHeight
}
