import QtQuick 2.3
import live 1.0

StaticFileReader{
    property var root : JSON.parse(data);
}
