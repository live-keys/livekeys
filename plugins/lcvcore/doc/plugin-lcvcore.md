# Plugin `lcvcore`

This module contains all the main qml elements for Live CV's computer vision code. In comparison to opencv, it is an
actual merge between the core and highgui modules.

```js
import lcvcore 1.0
```

{plugin:lcvcore}
{qmlSummary:lcvcore}


{qmlType:Mat}
{qmlInherits:base#Shared}
{qmlBrief:Main matrix object with variable types.}

  You can access a matrix's pixels from QML by using the buffer() function, which gives you a js ArrayBuffer. Here's a
  how you can access pixel values from a RGB matrix.

```qml
ImRead{
    id : src
    file : project.path + '/sample.jpg'
    Component.onCompleted : {
       var buffer     = output.buffer()
       var size       = output.dimensions()
       var channels   = output.channels()
       var bufferview = new Uint8Array(buffer)
       for ( var i = 0; i < size.height; ++i ){
           for ( var j = 0; j < size.width; ++j ){
               var b = bufferview[i * size.width + j * channels + 0]; // get the blue channel
               var g = bufferview[i * size.width + j * channels + 1]; // get the green channel
               var r = bufferview[i * size.width + j * channels + 2]; // get the red channel
           }
       }
    }
}
```
A sample on accessing and changing matrixes is available in **samples/core/customfilter.qml** :

{qmlEnum:Type}

Matrix type.

The matrix can be one of the following:


 * `Mat.CV8U`
 * `Mat.CV8S`
 * `Mat.CV16U`
 * `Mat.CV16S`
 * `Mat.CV32S`
 * `Mat.CV32F`
 * `Mat.CV64F`

{qmlMethod:ByteArray buffer()}

Returns an equivalent ArrayBuffer to access the matrix values

{qmlMethod:int channels()}

Returns the number of channels for the matrix

{qmlMethod:int depth()}

Returns the depth or type of the matrix

{qmlMethod:size dimensions()}

Returns the matrix dimensions

{qmlMethod:Mat cloneMat()}

Returns a cloned matrix with javascript ownership

{qmlMethod:Mat createOwnedObject()}

Returns a shallow copied matrix with javascript ownership


{qmlType:WritableMat}
{qmlInherits:base#Shared}
{qmlBrief:Writable matrix object, used mostly for quick operations like drawing}

{qmlMethod:ByteArray buffer()}

Returns an equivalent ArrayBuffer to access the matrix values

{qmlMethod:int channels()}

Returns the number of channels for the matrix

{qmlMethod:int depth()}

Returns the depth or type of the matrix

{qmlMethod:size dimensions()}

Returns the matrix dimensions

{qmlMethod:Mat toMat()}

Returns a [Mat](qml:lcvcore#Mat) object copy of the WritableMat.


{qmlType:MatOp}
{qmlInherits:external.QtQml#QtObject}
{qmlBrief:Singleton type for matrix operations.}

Main object used to create matrices and manage simple operations on them.

Creation examples:

```qml
import QtQuick 2.3
import lcvcore 1.0 as Cv

Grid{
    spacing: 2
    Cv.MatView{
         mat: {
             var m = Cv.MatOp.create(Qt.size(100, 100), Cv.Mat.CV8U, 3)
             Cv.MatOp.fill(m, "#003333")
             return m
         }
    }
    Cv.MatView{
         mat: Cv.MatOp.createFill(Qt.size(100, 100), Cv.Mat.CV8U, 3, "#660000")
    }
    Cv.MatView{
         mat: Cv.MatOp.createFromArray([
             [0,   100, 0],
             [150, 0,   250],
             [0,   200, 0]
         ])
         linearFilter: false
         width: 100
         height: 100
    }
}
```

{qmlMethod:Mat create(size size, Mat.Type type = Mat.CV8U, int channels = 1)}

Creates a matrix given the size, type and number of channels.

{qmlMethod:Mat createFill(size size, Mat.Type type = Mat.CV8U, int channels, color color)}

Creates a matrix given the size, type and number of channels, and fills it with the specified color.

{qmlMethod:Mat createFromArray(Array a, Mat.Type type = Mat.CV8U)}

Creates a matrix given a 2 dimensional array.

{qmlMethod:fill(Mat m, color color)}

Fills a matrix with the given color.

{qmlMethod:fillWithMask(Mat m, color color, Mat mask)}

Fills a matrix with the given color within the region of the binary mask.

{qmlMethod:Mat crop(Mat m, rect region)}

Crops a matrix by the specified region.

{qmlMethod:Matrix4x4 to4x4Matrix(Mat m)}

Returns a Matrix4x4 qml element from a given matrix `m`. `m` is required to be a `4x4` matrix.

{qmlMethod:Array toArray(Mat m)}

Returns the matrix `m` as a js Array.


{qmlType:MatView}
{qmlInherits:external.QtQuick#Item}
{qmlBrief:Displays a Mat as an image.}

{qmlProperty:Mat mat}

The matrix to be displayed.

{qmlProperty:bool linearFilter}

Smooths the displayed image through linear filtering. Default is `true`.

{qmlType:ImRead}
{qmlInherits:lcvcore#MatDisplay}
{qmlBrief:Read an image from the hard drive into a lcvcore.Mat structure.}

To read the image, all you need is to specify the location :

```qml
ImRead{
    file : 'sample.jpg'
}
```

You can load the image from the location where your qml file is saved by using the path from the `project` variable.

```qml
ImRead{
    file : project.dir() + '/../_images/caltech_buildings_DSCN0246.JPG'
}
```

{qmlProperty:string file}

The path to the file to load.


{qmlProperty:ImRead.Load isColor}

Color type of the image

Can be one of the following:

 * ```ImRead.CV_LOAD_IMAGE_UNCHANGED```
 * ```ImRead.CV_LOAD_IMAGE_GRAYSCALE```
 * ```ImRead.CV_LOAD_IMAGE_COLOR```
 * ```ImRead.CV_LOAD_IMAGE_ANYDEPTH```
 * ```ImRead.CV_LOAD_IMAGE_ANYCOLOR```

{qmlType:ImWrite}
{qmlInherits:external.QtQml#QtObject}
{qmlBrief:Saves an image to a specified file.}

{qmlProperty:Object params}

Parameters can be one of the following, also dependent on the format the image is saved in:

```
{
  'jpegQuality' : // jpeg compression
  'pngCompression' : // png compression
  'pxmBinary' : // For PPM, PGM, or PBM, it can be a binary format flag ( CV_IMWRITE_PXM_BINARY ), 0 or 1. Default value is 1.
}

```

{qmlType:MatDisplay}
{qmlInherits: Item}
{qmlBrief: Simple matrix display element.}

  This type serves as a base for all other live cv types that require displaying a matrix, which is available in its
  output property. You can choose wether smoothing occurs when scaling the image for display by enabling linear

{qmlMethod:saveImage(string file, Mat image)}

Save the `image` to a `file` path.

{qmlType:MatDisplay}
{qmlInherits:external.QtQuick#Item}
{qmlBrief: Simple matrix display element.}

  This type serves as a base for all other types that want to display a matrix, exposed in its
  `output` property. You can choose wether smoothing occurs when scaling the image by enabling linear
  filtering.

{qmlProperty: Mat MatDisplay output}

  This property holds the output element to display on screen.

{qmlProperty: bool MatDisplay linearFilter}

  If set to `true`, linear filtering will occur when scaling the image on the screen. Default value is `true`.

{qmlType:MatFilter}
{qmlInherits:lcvcore#MatDisplay}
{qmlBrief:Base filter for processing images.}

  Inherits the `MatDisplay` type in order to display a `Mat` through it's `output` property, which is the result of
  processing the `input`. This class doesn't have any processing implemented, however it serves as a base type for most
  transformations within lcv plugins.

{qmlProperty:Mat input}

  Input matrix to apply the filter to.

{qmlType:MatRoi}
{qmlInherits:lcvcore#MatFilter}
{qmlBrief:Selects a region of interest (ROI)}

Selects a region from an image for further processing. The 'PanAndZoom' component shows how to use a MatRoi to select
a region from an image, then use a MatRead to read the regions values.

See __*samples/imgproc/panandzoom.qml*__

{qmlProperty:int regionX}

The x coordinate of the most top-left point of the region.

{qmlProperty:int regionY}

The y coordinate of the most top-left point of the region.

{qmlProperty:int regionWidth}

The width of the seleted region.

{qmlProperty:int regionHeight}

The height of the seleted region.

{qmlType:MatRead}
{qmlInherits:external.QtQuick#Item}
{qmlBrief:Displays a matrixes values in text form}

Reads an image's values and displays them on the screen in the form of a grid. See the `PanAndZoom` component for more
details.

 See __*samples/imgproc/panandzoom.qml*__

An alternative to the given example is to use a flickable area and use the `MatRead` over a whole image.

{qmlProperty:int input}

Input matrix to read.

{qmlProperty:Font font}

Font to use when displaying values. Default is "Courier New" with 12 pixel size.
 
{qmlProperty:Color color}

Color of the displayed values

{qmlProperty:int numberWidth}

Number of digits to display for each number. Default is 3.

{qmlProperty:bool squareCell}

When enabled, each number cell will be resized to a square shape. This is useful if you want to display pixels
together with their values. Default is false.

{qmlType:MatBuffer}
{qmlInherits:lcvcore#MatDisplay}
{qmlBrief:Keeps the previous frame.}

The MatBuffer type is useful when it comes to video playback and you need to keep reference to a previous frame. The
buffer stores the frame for next usage, so by it's output you actually get the last frame that was passed around in
the application. It comes in handy in frame differences, as in the example under
__*samples/imgproc/framedifference.qml*__

{qmlProperty:Mat input}

Input matrix to store. Whenever a new input is given, the previous one becomes available as output.

{qmlType:CamCapture}
{qmlInherits:lcvcore#MatDisplay}
{qmlBrief:Captures frames from a connected camera. This is a _**static item**_.}

An example of how to use the cam capture can be found in _**samples/core/camcapture.qml**_.

{qmlProperty:string Device}

This property holds the url to the camera device to be accessed.

{qmlProperty:size resolution}

Stores the resolution for this cam capture.

{qmlProperty:real fps}

By default, this is initialized with the camera's given fps. You can change this value if you want faster/slower
capture playback, however, setting it faster than the actual camera permits will limit to the camera's maximum
rate. This value is not absolute, in fact it depends on a lot of factors like processing time done by Live CV and
speed by which frames are delivered.

{qmlmethod:CamCapture::staticLoad(string device, size resolution)}

This is an overloaded method for CamCapture staticLoad

{qmlMethod:CamCapture staticLoad(string device,size resolution)}

Loads the CamCaptures state, where `device` can be either a link or a device number. The device number should be given
in string form. Usually a default webcam can be accesed by '0'. `Resolution` is optional and stores the
resolution at which to open the capture.

{qmlProperty:bool paused}

This property can be set to true or false, depending if you want to freeze or continue capturing frames from the camera.


{qmlType:VideoCapture}
{qmlInherits:external.QtQuick#Item}
{qmlBrief:Captures frames from video files.This is a _**static item**_.}

The VideoCapture constantly grabes frames from a video file. The frames are captured at a speed equal to the video's
fps, but that is not necessarily to be considered as an absolute value. The speed can be altered manually by
configuring the fps parameter. A progress bar and a play/pause button can be attached by using the
[VideoControls](qml:lcvcore#VideoControls) type.

The first example in core/videocapture_simple.qml shows a simple video frame grabber, while the second one in
core/videocapture_controls.qml shows the grabber with the VideoControls attached.

* __*samples/core/videocapture_simple.qml*__

* __*samples/core/videocapture_controls.qml*__

{qmlProperty:Mat output}

Output frame.

{qmlProperty:bool linearFilter}

Perform linear filtering when scaling the matrix to be displayed. The default value is `true`.

{qmlProperty:float fps}

By default, this is initialized with the video files fps. You can change this value if you want faster/slower
capture playback.

{qmlProperty:bool loop}

If enabled, the video will start over once it's reach the end. By default, this value is `false`.

{qmlProperty:int CurrentFrame}

This property holds the current frame number. If you set this manually, you perform a seek to the specified frame
number in the video.

{qmlMethod:seek(int frame)}

Perform a seek to the specified [frame]() number.

{qmlProperty:string file}

This property holds the url to the file thats opened.

{qmlMethod:staticOpen(string file)}

This is an overloaded method for VideoCapture staticLoad

{qmlMethod:staticLoad(string file)}

Loads the VideoCapture state. `file` is a link to the file to be opened. This also acts a a state identifier.

{qmlProperty:bool paused}

Pause or play the video.

{qmlType:AlphaMerge}
{qmlInherits:lcvcore#MatFilter}
{qmlBrief:Merges an alpha channel to a matrix.}

Alpha merge is used to merge an alpha channel to a 1 or 3 channel matrix. The alpha channel is a single channel image
or mask that is loaded within the mask property of this class.

In the sample at samples/imgproc/alphamerge.qml a loaded image is merged with a drawn circle.

See **samples/imgproc/alphamerge.qml**

{qmlProperty:Mat mask}

Mask to merge the input with.

{qmlType:AbsDiff}
{qmlInherits:lcvcore#MatFilter}
{qmlBrief:Performs an absolute difference between two matrixes.}

The example in **samples/imgproc/framedifference.qml** shows differentiating two consecutive frames in a video to
calculate the motion. It uses a MatBuffer to store the previous frame:

**imgproc/framedifference.qml**

{qmlProperty:Mat input2}

Second input for the subtraction.

{qmlType:MatLoader}
{qmlInherits:external.QtQuick#item}
{qmlBrief:Static matrix loader}

{qmlMethod:Mat staticLoad(string id, Object options)}

Loads the matrix statically, where id is the static id used to capture the matrix, and options can be:

 * `w` : width of the matrix
 * `h` : height of the matrix
 * `ch` : no of channels for the matrix
 * `type` : matrix type
 * `color` : background color

{qmlType:ImageFile}
{qmlInherits:lcvcore#MatDisplay}
{qmlBrief:Read an image from the hard drive into a Mat structure.}

{qmlProperty:string file}

The path to the file to load.

{qmlProperty:enumaration isColor}

Color type of the image

Can be one of the following:

 * ```ImRead.CV_LOAD_IMAGE_UNCHANGED```
 * ```ImRead.CV_LOAD_IMAGE_GRAYSCALE```
 * ```ImRead.CV_LOAD_IMAGE_COLOR```
 * ```ImRead.CV_LOAD_IMAGE_ANYDEPTH```
 * ```ImRead.CV_LOAD_IMAGE_ANYCOLOR```

{qmlProperty:monitor}

Monitors the file for changes and reloads the image if the file has changed.

{qmlType:OverlapMat}
{qmlInherits:lcvcore#MatDisplay}
{qmlBrief:Overlaps 2 matrixes}

{qmlProperty:input2}

Mat to overlap with.

{qmlProperty:mask}

Mask used when overlaping.

{qmlType:ItemCapture}
{qmlInherits:lcvcore#MatDisplay}
{qmlBrief:Captures the screen into a Mat object.}

Available through the `output` property from `MatDisplay`.

{qmlProperty:Item captureSource}

Item to capture screen from.

{qmlType:VideoControls}
{qmlInherits:external.QtQuick#Rectangle}
{qmlBrief:Video controls provides a play/pause button and a seekbar for `VideoCapture`}

{qmlProperty:VideoCapture videoCapture}

The actual videoCapture object

{qmlSignal:playPauseTriggered(bool paused)}

Triggered when play/pause state changed

{qmlSignal:seekTriggered(int currentFrame)}

Triggered when a seek occurred


{qmlType:VideoWriter}
{qmlInherits:external.QtQuick#Item}
{qmlBrief:Writes video to a file. This is a _**static item**_.}

{qmlProperty:Mat input}

Input matrix to write. Whenever the input is set, the matrix will be written to a file.

{qmlProperty:int framesWritten}

Number of frames written. This is a read only property.

{qmlMethod:staticLoad(Object parameters)}

Loads the static object, where parameters is an object with the following keys:

 * `filename`: name of the file to write into
 * `fourcc` : a string based sequence of characters that describes the codec to be
 used
 * `fps`: fps of the video
 * `frameWidth` : video frame width
 * `frameHeight` : video frame height
 * `isColor`: boolean flag set to true if the encoder should work with color or greyscale
 frames. Default is true.

{qmlType:DrawHistogram}
{qmlInherits:external.QtQuick#Item}
{qmlBrief:Draws a histogram given from a set of points}

Example:

```

DrawHistogram{
    width : 200
    height : 200
    maxValue : 200
    colors : ['#aa007700', '#aa000088']
    render : DrawHistogram.ConnectedLines
    values : [
        [10, 44, 30, 50, 200, 10, 300, 50, 70],
        [20, 30, 40, 50, 100, 30, 200, 50, 30]
    ]
}
```

{qmlEnum:RenderType}

Type of histogram to render

 * `ConnectedLines` : Points will be connected by a set of lines
 * `Rectangles` : Rectangle or bar based histogram
 * `Binary`: Binary histogram
 * `BinaryConverted`: Int values are expected, which will be converted to binary

{qmlProperty:list colors}

List of colors associated with the indexes of each histogram graph.

{qmlProperty:list values}

List of graphs, each one consisting of a list of values for that graph.

{qmlProperty:real maxValue}

Max value shown in the histogram

{qmlProperty:DrawHistogram.RenderType render}

Render type of this histogram

{qmlMethod:setValuesFromIntList(list values)}

Assigns the values from a list of integers.

{qmlMethod:setValuesFromIntListAt(list values, int index))

Assigns values from the given list only at the specified index.

{qmlType:ColorHistogram}
{qmlInherits:external.QtQuick#Item}
{qmlBrief:Calculates histogram for a given matrix}

{qmlEnum:Selection}

Channel selection to calculate the histogram for the image.

 * `Total` : calculates the lightness of the image
 * `AllChannels` : Calculates all the channels
 * `BlueChannel` : Calculates the blue channel
 * `GreenChannel` : Calculates the green channel
 * `RedChannel` : Calculates the red channel

{qmlProperty:Mat input}

Input matrix to calculate the histogram for.

{qmlProperty:Mat output}

Output matrix containing the drawn histogram.

{qmlProperty:bool fill}

Flag that sets wether to fill the resulting graph.

{qmlProperty:int channel}

Channel to calculate the histogram for. Can be `ColorHistogram.AllChannels` for all the
channels, or `ColorHistogram.TotalChannels` to calculate the lightness.

