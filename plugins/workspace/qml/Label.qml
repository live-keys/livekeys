import QtQuick 2.3

Text{
    property QtObject textStyle: TextStyle{}
    font: textStyle.font
    color: textStyle.color
}
