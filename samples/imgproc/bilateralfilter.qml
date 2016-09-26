import lcvcore 1.0
import lcvimgproc 1.0

Row{
  
    CamCapture{
        id : cam
        device : '0'
    }

    BilateralFilter{
        input : cam.output
        d : 9
        sigmaColor : 100
        sigmaSpace : 100
    }
   
}

