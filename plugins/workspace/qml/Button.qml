import QtQuick 2.3

Loader{
    id: loader

    signal clicked()

    property Component content: null
    sourceComponent: content

}
