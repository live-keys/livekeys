import "lcvcore" 1.0
import "lcvimgproc" 1.0
import "lcvcontrols" 1.0

Row{
    
    id : root
    
    property string imagePath : codeDocument.path + '/../_images/caltech_buildings_DSCN0246.JPG'
    
    Rectangle{
        
        width : sc.width
        height : sc.height
        
        ImRead{
            id : sc
            file : root.imagePath
            Component.onCompleted : {
                width  = width / 4
                height = height / 4
            }
            RegionSelection{
                item : matRoi
                anchors.fill : parent
            }

        }
        
        Resize{
            id : scrs
            input : sc.output
            fx : 0.25
            fy : 0.25
        }
    }
    Rectangle{
        width : matRoi.width
        height : matRoi.height
        
    
        MatRoi{
            id : matRoi
            width : imgval.width
            height : imgval.height
            regionWidth : 20
            regionHeight : 20
            regionX : 0
            regionY : 0
            linearFilter: false
            input : scrs.output
        }
    
        MatRead{
            id : imgval
            input : matRoi.output
            opacity : 0.5
            squareCell : true
        }
    }    
}

