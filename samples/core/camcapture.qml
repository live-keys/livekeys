import QtQuick 2.3
import lcvcore 1.0

// A simple example showing how to use the
// camera capture device.

// Modify the 'deviceId' property in order
// to match the url of your webcam.

CamCapture{
    
    property string deviceId : '0'
    
    anchors.centerIn : parent
    device : deviceId
}
