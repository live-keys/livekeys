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




