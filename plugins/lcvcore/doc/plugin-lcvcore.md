# Plugin `lcvcore`

This module contains all the main qml elements for Live CV's computer vision code. In comparison to opencv, it is an
actual merge between the core and highgui modules.

```js
import lcvcore 1.0
```

{plugin:lcvcore}
{qmlSummary:lcvcore}



{qmlType:Mat}
{qmlInherits:QtQml.QtObject}
{qmlBrief:Main matrix object with variable types.}


{qmlEnum:Type}

Matrix type.

The matrix can be one of the following:


 * Mat.CV8U
 * Mat.CV8S
 * Mat.CV16U
 * Mat.CV16S
 * Mat.CV32S
 * Mat.CV32F
 * Mat.CV64F

{qmlMethod:ByteArray buffer()}

Returns an equivalent ArrayBuffer to access the matrix values

{qmlMethod:int channels()}

Returns the number of channels for the matrix

{qmlMethod:int depth()}

Returns the depth or type of the matrix

{qmlMethod:Size dimensions()}

Returns the matrix dimensions

{qmlMethod:Mat cloneMat()}

Returns a cloned matrix with javascript ownership

{qmlMethod:Mat createOwnedObject()}

Returns a shallow copied matrix with javascript ownership


{qmlType:MatView}
{qmlInherits:QtQuick.Item}
{qmlBrief:Provides a view for a lcvcore/Mat object}

{qmlProperty:bool linearFilter}

smooth the displayed image through linear filtering
{qmlType:ImRead}
{qmlInherits:lcvcore.MatDisplay}
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


{qmlProperty:enumaration isColor}

Color type of the image

Can be one of the following:

 * ```ImRead.CV_LOAD_IMAGE_UNCHANGED```
 * ```ImRead.CV_LOAD_IMAGE_GRAYSCALE```
 * ```ImRead.CV_LOAD_IMAGE_COLOR```
 * ```ImRead.CV_LOAD_IMAGE_ANYDEPTH```
 * ```ImRead.CV_LOAD_IMAGE_ANYCOLOR```

{qmlType:ImWrite}
{qmlInherits:QtObject}
{qmlBrief:Saves an image to a specified file.}

Parameters:	

* params

```
{
  'jpegQuality' : // jpeg compression
  'pngCompression' : // png compression
  'pxmBinary' : // For PPM, PGM, or PBM, it can be a binary format flag ( CV_IMWRITE_PXM_BINARY ), 0 or 1. Default value is 1.
}

```



{qmlType:MatRoi}
{qmlInherits:MatFilter}
{qmlBrief:Selects a region of interest (ROI)}

Usage available under _**samples/core/mat2darray.qml**_.

Select a region from an image for further processing. The 'PanAndZoom' component shows how to use a MatRoi to select
a region from an image, then use a MatRead to read the regions values.

 `imgproc/panandzoom.qml`

{qmlProperty:int regionX}

The x coordinate of the most top-left point of the region.

{qmlProperty:int regionY}

The y coordinate of the most top-left point of the region.

{qmlProperty:int regionWidth}

The width of the seleted region.

{qmlProperty:int regionHeight}

The height of the seleted region.

{qmlType:MatRead}
{qmlInherits:Item}
{qmlBrief:Displays a matrixes values in text form}

This element becomes very useful when debugging result images from algorithms. It can be used in combination
with the MatRoi element in order to create a pan-type interaction over a zoomed area in an image. The sample in
imgproc/panandzoom.qml uses this component.

 `imgproc/panandzoom.qml`

An alternative to the above example is to use a flickable area and use the MatRead over a whole image.

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

{qmlType:Mat2DArray}
{qmlInherits:MatDisplay}
{qmlBrief:Creates a matrix from a given array of values.}

{qmlProperty:Array values}

Array of values in the matrix form

{qmlProperty:lcvcore.Mat.Type}

Type of the matrix

{qmlType:MatBuffer}
{qmlInherits:MatDisplay}
{qmlBrief:Keeps the previous frame.}

The MatBuffer type is useful when it comes to video playback and you need to keep reference to a previous frame. The
buffer stores the frame for next usage, so by it's output you actually get the last frame that was passed around in
the application. It comes in handy in frame differences, as in the example under
samples/imgproc/framedifference.qml

 `imgproc/framedifference.qml`

{qmlProperty:QMat MatBuffer input}

Input matrix to store. Whenever a new input is given, the previous one becomes available as output.

{qmlType:MatEmpty}
{qmlInherits:MatDisplay}
{qmlBrief:Creates an empty matrix.}

Use this to create an empty matrix by spcecifying the size, background color, number of channels and type. The
drawing example shows how to create an empty matrix, then use the draw element to draw on its surface.

`imgproc/drawing.qml`

{qmlProperty:Size matSize}

The created matrix size.

{qmlProperty:Color color}

The background color of the created matrix.

{qmlProperty:int channels}

The number of channels of the created matrix.

{qmlProperty:Mat Type type}

The type of the created matrix.

{qmlType:CamCapture}
{qmlInherits:MatDisplay}
{qmlBrief:Captures frames from a connected camera. This is a _**static item**_.}

An example of how to use the cam capture can be found in _**samples/core/camcapture.qml**_ :

 `core/camcapture.qml`

{qmlProperty:string Device}

This property holds the url to the camera device to be accessed.

{qmlProperty:size resolution}

Stores the resolution for this cam capture.

{qmlProperty:real fps}

By default, this is initialized with the camera's given fps. You can change this value if you want faster/slower
capture playback, however, setting it faster than the actual camera permits will limit to the camera's maximum
rate. This value is not absolute, in fact it depends on a lot of factors like processing time done by Live CV and
speed by which frames are delivered.

{qmlmethod CamCapture::staticLoad(string device, size resolution)}

This is an overloaded method for CamCapture::staticLoad

{qmlMethod:CamCapture staticLoad(string device,size resolution)}

Loads the CamCaptures state. [device]() can be either a link or a device number. The device number should be given
in string form. Usually a default webcam can be accesed by the '0'. [Resolution]() is optional and stores the
resolution at which to open the capture.

{qmlProperty:bool paused}

This property can be set to true or false, depending if you want to freeze or continue capturing frames from the camera.


{qmlType:VideoCapture}
{qmlInherits:Item}
{qmlBrief:Captures frames from video files.This is a _**static item**_.}

The VideoCapture constantly grabes frames from a video file. The frames are captured at a speed equal to the video's
fps, but that is not necessarily to be considered as an absolute value. The speed can be altered manually by
configuring the fps parameter. A progress bar and a play/pause button can be attached by using the VideoControls type
in the lcvcontrols module.

The first example in core/videocapture_simple.qml shows a simple video frame grabber, while the second one in
core/videocapture_controls.qml shows the grabber with the VideoControls attached.

* ```core/videocapture_simple.qml```

* ```core/videocapture_controls.qml```

{qmlProperty:Mat VideoCapture output}

Output frame.

{qmlProperty:bool linearFilter}

Perform linear filtering when scaling the matrix to be displayed. The default value is true.

{qmlProperty:float fps}

By default, this is initialized with the video files fps. You can change this value if you want faster/slower
capture playback.

{qmlProperty:bool loop}

If enabled, the video will start over once it's reach the end. By default, this value is disabled.

{qmlProperty:int CurrentFrame}

This property holds the current frame number. If you set this manually, you perform a seek to the specified frame
number in the video.

{qmlMethod:VideoCapture seek(int frame)}

Perform a seek to the specified [frame]() number.

{qmlProperty:string file}

This property holds the url to the file thats opened.

{qmlMethod:VideoCapture staticOpen(string file)}

This is an overloaded method for VideoCapture::staticLoad

{qmlMethod:VideoCapture staticLoad(string file)}

Loads the VideoCapture state. [file]() is a link to the file to be opened. This also acts a a state identifier.

{qmlProperty:bool paused}

Pause / play the video by setting this property to true or false.

{qmlType:AlphaMerge}
{qmlInherits:MatFilter}
{qmlBrief:Merges an alpha channel to a matrix.}

Alpha merge is used to merge an alpha channel to a 1 or 3 channel matrix. The alpha channel is a single channel image
or mask that is loaded within the mask property of this class.

In the sample at samples/imgproc/alphamerge.qml a loaded image is merged with a drawn circle.

 `imgproc/alphamerge.qml`

{qmlProperty:Mat mask}

Mask to merge the input with.

{qmlType:AbsDiff}
{qmlInherits:MatFilter}
{qmlBrief:Performs an absolute difference between two matrixes.}

The example in **samples/imgproc/framedifference.qml** shows differentiating two consecutive frames in a video to
calculate the motion. It uses a MatBuffer to store the previous frame :

 `quotefile imgproc/framedifference.qml`

{qmlProperty:Mat input2}

Second input for the subtraction.

{qmlType:MatList}
{qmlBrief:Matrix list that can be used as a model.}

* qmlMethod appendMat(Mat mat) appends a matrix to the list
* qmlMethod removeMat(Mat mat) removes the matrix mat from the list
* qmlMethod removeAt(int index) removes matrix at index
* qmlMethod Mat at(int index) returns the matrix at index
* qmlMethod int size() returns the total matrixes in the list

{qmlType:MatLoader}
{qmlBrief:Static matrix loader}

qmlMethod Mat staticLoad(string id, Object options)
Loads the matrix statically, where id is the static id used to capture the matrix, and options can be:

```
{
 'w' : // width of the matrix
 'h' : // height of the matrix
 'ch' : // no of channels of the matrix
 'type' : // matrix type
 'color' : // default background color
}

```

{qmlType:ImageFile}
{qmlInherits:MatDisplay}
{qmlBrief:Read an image from the hard drive into a lcvcore.Mat structure.}

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
{qmlInherits:MatDisplay}
{qmlBrief:Overlaps 2 matrixes}

{qmlProperty:input2}

Mat to overlap with

{qmlProperty:mask}

Mask used when overlaping

{qmlType:ItemCapture}
{qmlInherits:MatDisplay}
{qmlBrief:Captures the screen into a Mat object.}

Available through the `output` property from `MatDisplay`.

{qmlProperty:Item captureSource}

Item to capture screen from

{qmlType:VideoControls}
{qmlBrief:Video controls provides a play/pause button and a seekbar for a `lcvcore/VideoCapture`}

{qmlProperty:VideoCapture videoCapture}
{qmlBrief: receives the actual videoCapture object}

{qmlSignal:playPauseTriggered(bool paused)}

Triggered when play/pause state changed

{qmlSignal:seekTriggered(int currentFrame)}

Triggered when a seek occurred





