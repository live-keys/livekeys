import QtQuick 2.3
import live 1.0
import base 1.0
import lcvcore 1.0 as Cv
import lcvimgproc 1.0

Column{  
    
    Cv.ImRead{ 
        id: src
        file: project.path('../../../samples/_images/buildings_0246.jpg')
    }
    
    RemoteLine{
        id: rl
        
        connection : Fork{}
        
        property var input: src.output.cloneMat()
        
        source : ComponentSource{
            id: cs
            
           GaussianBlur{
                id: gaussianBlur
                input: post.input ? post.input : Cv.MatOp.nullMat
                onOutputChanged: {
                    response.send('blurred', output)
                }
                ksize: "5x5"
                sigmaX: 5
                sigmaY: 5
            }
        }
    }
    
    Cv.MatView{
        id: matView
        mat: rl.result ? rl.result.blurred : null
    }
    
}
