Rectangle{
    MatSource{
        id : src
        file : todo
        Component.onCompleted : {
            width = width / 4
            height = height / 4
        }
    }
    Sobel{
      input : src.output
      ddepth : Mat.CV16S
      width : src.width * 3
      height : src.height * 3
      xorder : 1
      yorder : 0
      ksize  : 3
      scale  : 1
      delta : 0
    }
}
