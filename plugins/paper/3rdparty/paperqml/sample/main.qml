import QtQuick 2.6
import QtQuick.Window 2.2

Window {
    visible: true
    width: 800
    height: 550
    title: qsTr("Paperjs Sample")

    Column{
        width: parent.width
        height: parent.height

        // Creating a simple path

        PaperCanvas{
            id: paperCanvas
            width: parent.width
            height: 100

            onPaperReady: {
                var path = new paper.Path()
                path.strokeColor = '#ff0000'
                var start = new paper.Point(100, 100)
                path.moveTo(start);
                path.lineTo(start.add([ 200, -50 ]));
                paperCanvas.paint()
            }
        }

        Rectangle{
            width: parent.width
            height: 2
            color: '#ccc'
        }

        // Using a MosueArea to control paperevents

        PaperCanvas{
            id: paperCanvas2
            width: parent.width
            height: 100

            MouseArea{
                anchors.fill: parent

                property var path : null

                onClicked: {
                    var paper = paperCanvas2.paper
                    if ( !path ){
                        path = new paper.Path();
                        path.strokeColor = '#000';
                        path.moveTo([mouse.x, mouse.y]);
                    } else {
                        path.lineTo([mouse.x, mouse.y]);
                    }
                    paperCanvas2.paint()
                }
            }
        }

        Rectangle{
            width: parent.width
            height: 2
            color: '#ccc'
        }

//      PaperCanvas defines a timer to start animations

        PaperCanvas{
            id: paperCanvas3
            width: parent.width
            height: 200

            onPaperReady: {
                var path = new paper.Path.Rectangle({
                    point: [75, 75],
                    size: [75, 75],
                    strokeColor: 'black'
                });

                paper.view.onFrame = function(event){
                    path.rotate(3);
                }
            }

        }
    }
}
