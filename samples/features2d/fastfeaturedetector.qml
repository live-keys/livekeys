import 'lcvcore' 1.0
import "lcvimgproc" 1.0
import "lcvfeatures2d" 1.0

Row{
    
    id : mainRow
    
    // Sample showing how to use 
    // Canny Edge Detector algorithm
    
    property string imagePath : codeDocument.path + '/../_images/caltech_buildings_DSCN0246_small.JPG'
    
    Component.onCompleted : {
        var ttt    = ImRead.createObject()
        var mmread = Qt.createQmlObject('ImRead');
        var sprite = mmread.createObject(mainRow, {"x": 100, "y": 100, 'file' : imagePath});
        if ( sprite == null )
            console.log('Error creating object.');
        console.log(mmread.errorString())
        console.log(mmread.status == Component.Error)
        /*if (mmread.status == Component.Ready){
            console.log('ready');
        } else {
            console.log('not ready')
            mmread.statusChanged.connect(function(){
                console.log('finally ready')
            });
        }*/
    }
    
}