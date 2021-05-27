import QtQuick 2.3

QtObject{
    property string workingDirectory: project.dir()
    property string highlightedFile: ''
    property string selectedFile: ''
}
