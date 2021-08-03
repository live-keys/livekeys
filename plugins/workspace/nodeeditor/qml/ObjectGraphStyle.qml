import QtQuick 2.3

QtObject{
    property color backgroundColor: '#000511'
    property color backgroundGridColor: '#222'

    property color borderColor: '#333'
    property color highlightBorderColor: '#555'
    property double borderWidth: 1
    property double radius: 5

    property color connectorEdgeColor: '#666'
    property color connectorColor: '#666'

    property color selectionColor: "#fff"
    property double selectionWeight: 1

    property QtObject objectNodeStyle : ObjectNodeStyle{}
    property QtObject objectNodeMemberStyle : ObjectNodeMemberStyle{}
}
