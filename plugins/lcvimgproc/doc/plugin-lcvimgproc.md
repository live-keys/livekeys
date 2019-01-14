# Plugin `lcvimgproc`

  This module contains elements for image processing. It resembles open cv's imgproc module.

```js
import lcvimgproc 1.0
```

{plugin:lcvimgproc}
{qmlSummary:lcvimgproc}


{qmlType:Blur}
{qmlInherits:MatFilter}
{qmlBrief:Blurs an image using the normalized box filter.}

  The function smoothes an image using the normalized box filter.

`quotefile imgproc/blur.qml`

{qmlProperty:Size ksize}

  Blurring kernel size.

{qmlProperty:Point anchor}

  Anchor point; default value Point(-1,-1) means that the anchor is at the kernel center.

{qmlProperty:int borderType}

  Border mode used to extrapolate pixels outside of the image.

See also [CopyMakeBorder::BorderType]()

{qmlType:Canny}
{qmlInherits:MatFilter}
{qmlBrief:Finds edges within an image.}

  Canny finds edges in the input image image and marks them in the output map edges using the Canny algorithm.
  The smallest value between threshold1 and threshold2 is used for edge linking. The largest value is used to find
  initial segments of strong edges

`quotefile imgproc/canny.qml`

{qmlProperty:int threshold1}

  First threshold for the hysteresis procedure.

{qmlProperty:Size threshold2}

  Second threshold for the hysteresis procedure.

{qmlProperty:Size apertureSize}

  Aperture size for the Sobel() operator.

{qmlProperty:Size l2gradient}

  A flag indicating whether a more accurate L2 norm should be used to calculate the image gradient magnitude.

{qmlType:GaussianBlur}
{qmlInherits:MatFilter}
{qmlBrief:Blurs an image using a Gaussian filter.}

`imgproc/gaussianblur.qml`

{qmlProperty:size ksize}

  Gaussian kernel size. ksize.width and ksize.height can differ but they both must be positive and odd. Or, they can be
  zero’s and then they are computed from sigma.

{qmlProperty:real sigmaX}

  Gaussian kernel standard deviation in X direction.

{qmlProperty:real sigmaY}

  Gaussian kernel standard deviation in Y direction.

{qmlProperty:int borderType}

  Pixel extrapolation method (see [CopyMakeBorder::BorderType]() for details).

{qmlType:HoughLinesP}
{qmlInherits:MatFilter}
{qmlBrief:Finds line segments in a binary image using the probabilistic Hough transform.}

`imgproc/houghlinesp.qml`


{qmlProperty:real rho}

  Distance resolution of the accumulator in pixels.

{qmlProperty:real theta}

  Angle resolution of the accumulator in radians.

{qmlProperty:int threshold}

  Accumulator threshold parameter. Only those lines are returned that get enough votes ( > threshold ).

{qmlProperty:real minLineLength}

  Minimum line length. Line segments shorter than that are rejected.

{qmlProperty:real maxLineGap}

  Maximum allowed gap between points on the same line to link them.

{qmlProperty:color lineColor}

  Color of the lines being drawn.

{qmlProperty:int lineThickness}

  Thickness of the lines being drawn.

{qmlType:HoughLines}
{qmlInherits:MatFilter}
{qmlBrief:Finds line segments in a binary image using the Hough transform.}

`imgproc/houghlines.qml`

{qmlproperty:real rho}

  Distance resolution of the accumulator in pixels.

{qmlproperty:real theta}

  Angle resolution of the accumulator in radians.

{qmlProperty:int threshold}

  Accumulator threshold parameter. Only those lines are returned that get enough votes ( > threshold ).

{qmlProperty:real srn}

  For the multi-scale Hough transform, it is a divisor for the distance resolution rho . The coarse accumulator distance
  resolution is rho and the accurate accumulator resolution is rho/srn . If both srn=0 and stn=0 , the classical Hough
  transform is used. Otherwise, both these parameters should be positive.

{qmlProperty:real stn}

  For the multi-scale Hough transform, it is a divisor for the distance resolution theta.

{qmlProperty:color lineColor}

  Color of the lines being drawn.

{qmlProperty:int lineThickness}

  Thickness of the lines being drawn.

{qmlType:Sobel}
{qmlInherits:MatFilter}
{qmlBrief:Calculates the first, second, third, or mixed image derivatives using an extended Sobel operator.}

`imgproc/sobeloperator.qml`


{qmlProperty:Mat Type ddepth}

  Output image depth, the following combinations of input depth and ddepth are supproted:

  * input.depth = Mat.CV8U, ddepth = -1 / Mat.CV16S / Mat.CV32F / Mat.CV64F
  * input.depth = Mat.CV16U / Mat.CV16S, ddepth = -1 / Mat.CV32F / Mat.CV64F
  * input.depth = Mat.CV32F, ddepth = -1 / Mat.CV32F / Mat.CV64F
  * input.depth = Mat.64F, ddepth = -1 / Mat.CV64F

  When ddepth = -1, the destination image will have the same depth as the source. In case of 8-bit
  input images, it will result in truncated derivatives.


{qmlProperty:int xorder}

  Order of the derivative in x.

{qmlProperty:int yorder}

  Order of the derivative in y.

{qmlProperty:int ksize}

  Size of the extended Sobel kernel; ( must be 1, 3, 5, 7 )

{qmlProperty:real scale}

  Optional scale factor for the computed derivative values; by default, no scaling is  applied.

{qmlProperty:real delta}

  Optional delta value that is added to the results prior to storing them in output.

{qmlProperty:real borderType}

  Pixel extrapolation method.

See also [CopyMakeBorder::BorderType](#BorderType)

{qmlType:CvtColor}
{qmlInherits:MatFilter}
{qmlBrief:Converts an image from one color space to another.}

`imgproc/threshold.qml`

{qmlProperty:enumeration CvtType}

  Conversion method:

  * CV_BGR2BGRA
  * CV_RGB2RGBA
  * CV_BGRA2BGR
  * CV_RGBA2RGB

  * CV_BGR2RGBA
  * CV_RGB2BGRA

  * CV_RGBA2BGR
  * CV_BGRA2RGB

  * CV_BGR2RGB
  * CV_RGB2BGR

  * CV_BGRA2RGBA
  * CV_RGBA2BGRA

  * CV_BGR2GRAY
  * CV_RGB2GRAY
  * CV_GRAY2BGR
  * CV_GRAY2RGB
  * CV_GRAY2BGRA
  * CV_GRAY2RGBA
  * CV_BGRA2GRAY
  * CV_RGBA2GRAY

  * CV_BGR2BGR565
  * CV_RGB2BGR565
  * CV_BGR5652BGR
  * CV_BGR5652RGB

  * CV_BGRA2BGR565
  * CV_RGBA2BGR565
  * CV_BGR5652BGRA
  * CV_BGR5652RGBA
  * CV_GRAY2BGR565
  * CV_BGR5652GRAY

  * CV_BGR2BGR555
  * CV_RGB2BGR555
  * CV_BGR5552BGR
  * CV_BGR5552RGB
  * CV_BGRA2BGR555
  * CV_RGBA2BGR555
  * CV_BGR5552BGRA
  * CV_BGR5552RGBA
  * CV_GRAY2BGR555
  * CV_BGR5552GRAY

  * CV_BGR2XYZ
  * CV_RGB2XYZ
  * CV_XYZ2BGR
  * CV_XYZ2RGB

  * CV_BGR2YCrCb
  * CV_RGB2YCrCb
  * CV_YCrCb2BGR
  * CV_YCrCb2RGB

  * CV_BGR2HSV
  * CV_RGB2HSV

  * CV_BGR2Lab
  * CV_RGB2Lab

  * CV_BayerBG2BGR
  * CV_BayerGB2BGR
  * CV_BayerRG2BGR
  * CV_BayerGR2BGR

  * CV_BayerBG2RGB
  * CV_BayerGB2RGB
  * CV_BayerRG2RGB
  * CV_BayerGR2RGB

  * CV_BGR2Luv
  * CV_RGB2Luv
  * CV_BGR2HLS
  * CV_RGB2HLS

  * CV_HSV2BGR
  * CV_HSV2RGB

  * CV_Lab2BGR
  * CV_Lab2RGB
  * CV_Luv2BGR
  * CV_Luv2RGB
  * CV_HLS2BGR
  * CV_HLS2RGB

  * CV_BayerBG2BGR_VNG
  * CV_BayerGB2BGR_VNG
  * CV_BayerRG2BGR_VNG
  * CV_BayerGR2BGR_VNG

  * CV_BayerBG2RGB_VNG
  * CV_BayerGB2RGB_VNG
  * CV_BayerRG2RGB_VNG
  * CV_BayerGR2RGB_VNG

  * CV_BGR2HSV_FULL
  * CV_RGB2HSV_FULL
  * CV_BGR2HLS_FULL
  * CV_RGB2HLS_FULL

  * CV_HSV2BGR_FULL
  * CV_HSV2RGB_FULL
  * CV_HLS2BGR_FULL
  * CV_HLS2RGB_FULL

  * CV_LBGR2Lab
  * CV_LRGB2Lab
  * CV_LBGR2Luv
  * CV_LRGB2Luv

  * CV_Lab2LBGR
  * CV_Lab2LRGB
  * CV_Luv2LBGR
  * CV_Luv2LRGB

  * CV_BGR2YUV
  * CV_RGB2YUV
  * CV_YUV2BGR
  * CV_YUV2RGB

  * CV_BayerBG2GRAY
  * CV_BayerGB2GRAY
  * CV_BayerRG2GRAY
  * CV_BayerGR2GRAY

  * CV_YUV2RGB_NV12
  * CV_YUV2BGR_NV12
  * CV_YUV2RGB_NV21
  * CV_YUV2BGR_NV21
  * CV_YUV420sp2RGB
  * CV_YUV420sp2BGR

  * CV_YUV2RGBA_NV12
  * CV_YUV2BGRA_NV12
  * CV_YUV2RGBA_NV21
  * CV_YUV2BGRA_NV21
  * CV_YUV420sp2RGBA
  * CV_YUV420sp2BGRA
  * CV_YUV2RGB_YV12
  * CV_YUV2BGR_YV12
  * CV_YUV2RGB_IYUV
  * CV_YUV2BGR_IYUV
  * CV_YUV2RGB_I420
  * CV_YUV2BGR_I420
  * CV_YUV420p2RGB
  * CV_YUV420p2BGR

  * CV_YUV2RGBA_YV12
  * CV_YUV2BGRA_YV12
  * CV_YUV2RGBA_IYUV
  * CV_YUV2BGRA_IYUV
  * CV_YUV2RGBA_I420
  * CV_YUV2BGRA_I420
  * CV_YUV420p2RGBA
  * CV_YUV420p2BGRA

  * CV_YUV2GRAY_420
  * CV_YUV2GRAY_NV21
  * CV_YUV2GRAY_NV12
  * CV_YUV2GRAY_YV12
  * CV_YUV2GRAY_IYUV
  * CV_YUV2GRAY_I420
  * CV_YUV420sp2GRAY
  * CV_YUV420p2GRAY

  * CV_YUV2RGB_UYVY
  * CV_YUV2BGR_UYVY
  * CV_YUV2RGB_Y422
  * CV_YUV2BGR_Y422
  * CV_YUV2RGB_UYNV
  * CV_YUV2BGR_UYNV

  * CV_YUV2RGBA_UYVY
  * CV_YUV2BGRA_UYVY
  * CV_YUV2RGBA_Y422
  * CV_YUV2BGRA_Y422
  * CV_YUV2RGBA_UYNV
  * CV_YUV2BGRA_UYNV

  * CV_YUV2RGB_YUY2
  * CV_YUV2BGR_YUY2
  * CV_YUV2RGB_YVYU
  * CV_YUV2BGR_YVYU
  * CV_YUV2RGB_YUYV
  * CV_YUV2BGR_YUYV
  * CV_YUV2RGB_YUNV
  * CV_YUV2BGR_YUNV

  * CV_YUV2RGBA_YUY2
  * CV_YUV2BGRA_YUY2
  * CV_YUV2RGBA_YVYU
  * CV_YUV2BGRA_YVYU
  * CV_YUV2RGBA_YUYV
  * CV_YUV2BGRA_YUYV
  * CV_YUV2RGBA_YUNV
  * CV_YUV2BGRA_YUNV

  * CV_YUV2GRAY_UYVY
  * CV_YUV2GRAY_YUY2
  * CV_YUV2GRAY_Y422
  * CV_YUV2GRAY_UYNV
  * CV_YUV2GRAY_YVYU
  * CV_YUV2GRAY_YUYV
  * CV_YUV2GRAY_YUNV

  * CV_RGBA2mRGBA
  * CV_mRGBA2RGBA

  * CV_COLORCVT_MAX

{qmlProperty: CvtType code}

  Color space conversion code.

{qmlProperty:int dstCn}

  Number of channels in the destination image; if the parameter is 0, the number of the channels is derived
  automatically from input and code.

{qmlType:Threshold}
{qmlInherits:MatFilter}
{qmlBrief:Applies a fixed-level threshold to each element.}

  The function applies fixed-level thresholding to a single-channel array. The function is typically used to get a
  bi-level (binary) image out of a grayscale image ( compare() could be also used for this purpose) or for removing a
  noise, that is, filtering out pixels with too small or too large values.

`imgproc/threshold.qml`


{qmlProperty:enumeration Type}

  Extrapolation method border type:
  
  * Threshold.BINARY if ( input(x, y) > thresh ) output(x, y) = maxVal; else output(x, y) = 0
  * Threshold.BINARY_INV if ( input(x, y) > thresh ) output(x, y) = 0; else output(x, y) = maxVal
  * Threshold.TRUNC if ( input(x, y) > thresh ) output(x, y) = threshold; else output(x, y) = input(x, y)
  * Threshold.TOZERO if ( input(x, y) > thresh ) output(x, y) = input(x, y); else output(x, y) = 0
  * Threshold.TOZERO_INV if ( input(x, y) > thresh ) output(x, y) = 0; else output(x, y) = input(x, y)
  * Threshold.OTSU The function determines the optimal threshold value using the Otsu’s algorithm and uses it instead
   of the specified thresh


{qmlProperty:int thresh}

  Threshold value

{qmlProperty:int maxVal}

  Maximum value to use with the THRESH_BINARY and THRESH_BINARY_INV thresholding types.

{qmlProperty: Type thresholdType}

  Maximum value to use with the THRESH_BINARY and THRESH_BINARY_INV thresholding types.

{qmlType:ChannelSelect}
{qmlInherits:MatFilter}
{qmlBrief:Selects an image channel by its index.}

`imgproc/framedifference.qml`

{qmlProperty:int channel}

  Channel number.

{qmlType:StructuringElement}
{qmlInherits:Item}
{qmlBrief:Creates a structuring element of the specified size and shape for morphological operations.
}

`imgproc/erodedilate.qml`


{qmlProperty: ElementShape shape}

  Element shape.
 
{qmlProperty:size ksize}

  Size of the structuring element.

{qmlProperty:point anchor}

  Anchor position within the element. The default value (-1, -1) means that the anchor is at the center. Note that only
  the shape of a cross-shaped element depends on the anchor position. In other cases the anchor just regulates how much
  the result of the morphological operation is shifted

{qmlProperty:Mat output}

  Created element.

{qmlProperty:enumeration ElementShape}

  Extrapolation method border type:

  * StructuringElement.MORPH_RECT A rectangluar structuring element.
  * StructuringElement.MORPH_ELLIPSE  An elliptic structuring element, that is, a filled ellipse inscribed into the
  rectangle Rect(0, 0, ksize.width, ksize.height)
  * StructuringElement.MORPH_CROSS A cross-shaped structuring element.

{qmlType:Resize}
{qmlInherits:MatFilter}
{qmlBrief:Resizes an image.}

  Resizes an image according to the specified filter.

`imgproc/resize.qml`

{qmlProperty:Size matSize}

  Size of the resized matrix. An unspecified size or a size of (0, 0) will make the element look into the fx and fy resize factors.

{qmlProperty:real fx}

  Factor by which to resize on x axis.

{qmlProperty:real fy}

  Factor by which to resize on y axis.

{qmlProperty:Interpolation interpolation}

  Interpolation method.

{qmlProperty:enumeration Interpolation}

  Interpolation method:

  * Resize.INTER_NEAREST nearest neighbor interpolation
  * Resize.INTER_LINEAR bilinear interpolation
  * Resize.INTER_CUBIC bicubic interpolation
  * Resize.INTER_AREA area-based (or super) interpolation
  * Resize.INTER_LANCZOS4 Lanczos interpolation over 8x8 neighborhood
  * Resize.INTER_MAX nearest neighbor interpolation*
  * Resize.WARP_INVERSE_MAP nearest neighbor interpolation


{qmlType:Filter2D}
{qmlInherits:MatFilter}
{qmlBrief:Converts an image with the specified kernel.}

`imgproc/filter2D.qml`


{qmlProperty:int ddepth}

  Desired depth of the destination image; if it is negative, it will be the same as input.depth(); the following
  combinations of src.depth() and ddepth are supported:

    * input.depth() = Mat.CV_8U, ddepth = -1/Mat.CV_16S/Mat.CV_32F/Mat.CV_64F
    * input.depth() = Mat.CV_16U/CV_16S, ddepth = -1/Mat.CV_32F/Mat.CV_64F
    * input.depth() = Mat.CV_32F, ddepth = -1/Mat.CV_32F/Mat.CV_64F
    * input.depth() = Mat.CV_64F, ddepth = -1/Mat.CV_64F

   where ddepth = -1 will yield an output the same as the source.

{qmlProperty:Mat kernel}

  Convolution kernel (or rather a correlation kernel), a single-channel floating point matrix.

{qmlProperty:Point anchor}

  Anchor of the kernel that indicates the relative position of a filtered point within the kernel; the anchor should lie
  within the kernel; default value (-1,-1) means that the anchor is at the kernel center.

{qmlProperty:real delta}

  Optional value added to the filtered pixels before storing them in Filter2D.output.
  
{qmlProperty:int borderType}

  Pixel extrapolation method (see [CopyMakeBorder::BorderType]() for details).



{qmlType: Dilate}
{qmlInherits:MatFilter}
{qmlBrief:Erodes an image by using a specific structuring element.}

 `imgproc/erodedilate.qml`

{qmlProperty:Mat kernel}

  Structuring element used for dilation; if element=Mat(), a  3x3 rectangular structuring element is used.

{qmlProperty:Point anchor}

  Position of the anchor within the element; default value (-1, -1) means that the anchor is at the element center.

{qmlProperty:int iterations}

  Number of times dilation is applied.

{qmlProperty:int borderType}

  Pixel extrapolation method.

  See also [CopyMakeBorder::BorderType]()

{qmlProperty:color borderValue}

  Border value in case of a constant border

  See also [CopyMakeBorder::BorderType]()

{qmlType:Erode}
{qmlInherits:MatFilter}
{qmlBrief:Dilates an image by using a specific structuring element.}

`imgproc/erodedilate.qml`


{qmlProperty:Mat kernel}

  Structuring element used for erosion; if element=Mat() , a 3x3 rectangular structuring element is used.

{qmlProperty:Point anchor}

  Position of the anchor within the element; default value (-1, -1) means that the anchor is at the element center.

{qmlProperty:int iterations}

  Number of times erosion is applied.

{qmlProperty:int borderType}

  Pixel extrapolation method.

  See also [CopyMakeBorder::BorderType]()

{qmlProperty:color borderValue}

  Border value in case of a constant border

  See also [CopyMakeBorder::BorderType]()

{qmlType:CopyMakeBorder}
{qmlInherits:MatFilter}
{qmlBrief:Copies the source image into the middle of the destination image.}

  The areas to the left, to the right, above and below the copied source image will be filled with extrapolated pixels

`imgproc/copymakeborder.qml`

{qmlProperty:enumeration BorderType}

  Extrapolation method border type:

  * CopyMakeBorder.BORDER_REPLICATE
  * CopyMakeBorder.BORDER_CONSTANT
  * CopyMakeBorder.BORDER_REFLECT
  * CopyMakeBorder.BORDER_WRAP
  * CopyMakeBorder.BORDER_REFLECT_101
  * CopyMakeBorder.BORDER_TRANSPARENT
  * CopyMakeBorder.BORDER_DEFAULT
  * CopyMakeBorder.BORDER_ISOLATED

{qmlProperty:int top}

  Property specifying how many pixels in the top direction from the source image rectangle to extrapolate.


{qmlProperty:int bottom}

  Property specifying how many pixels in the bottom direction from the source image rectangle to extrapolate.

{qmlProperty:int left}

  Property specifying how many pixels in the left direction from the source image rectangle to extrapolate.

{qmlProperty:int right}

  Property specifying how many pixels in the right direction from the source image rectangle to extrapolate.

{qmlProperty:int borderType}

  Border type.
  See also [CopyMakeBorder::BorderType]()

{qmlProperty:color color}

  Border value if borderType==CopyMakeBorder.BORDER_CONSTANT.
  See also [CopyMakeBorder::BorderType]()

{qmlType:MatDraw}
{qmlInherits:MatDisplay}
{qmlBrief:Provides matrix drawing operations.}

  Provides the basic set of functions needed in order to draw on a matrix.

`imgproc/drawing.qml`

{qmlProperty:Mat input}

  Input surface to draw on.

{qmlMethod:MatDraw cleanUp()}

  Cleans up the matrix surface to it's initial value.


{qmlMethod:MatDraw line(Point p1, Point p2, Color color, int thickness, int lineType, int shift)}

  Draw a line from point [p1]() to point [p2]() of a specified [color]() and [thickness](). The [lineType]() can be
  4 ( 4-connected line ), 8 (8-connected line, also the default) and -1 (antialiased line). The shift is the number
  of fractional bits in the point coordinates.

{qmlBrief:Draws a line from point [p1]() to point [p2]().}

  Parameters :
  * [color]()
  * [thickness]()
  * [lineType]()
  * [shift]()
  See also [MatDraw::line]()

{qmlMethod:MatDraw rectangle(Point p1, Point p2, Color color, int thickness, int lineType, int shift)}

  Draw a rectangle from point [p1]() to point [p2]() of a specified [color]() and [thickness](). The [lineType]() can be
  4 ( 4-connected line ), 8 (8-connected line, also the default) and -1 (antialiased line). The shift is the number
  of fractional bits in the point coordinates.

{qmlBrief:Draws a rectangle point [p1]() to point [p2]().}

  Parameters :
  * [color]()
  * [thickness]()
  * [lineType]()
  * [shift]()
  See also [MatDraw::rectangle]()

{qmlMethod:MatDraw circle(Point center, int radius, Color color, int thickness, int lineType, int shift)}

  Draw a circle by specifying its [center point](), [radius](), [color]() and [thickness](). For the lineType and shift, see
  the lin method.

  See also [MatDraw::line]()

{qmlBrief:Draws a circle in [center]() with [radius]().}

  Parameters :
  * [color]()
  * [thickness]()
  * [lineType]()
  * [shift]()

{qmlMethod:MatDraw ellipse(Point center, Size axes, real angle, real startAngle, real endAngle, Color color,
  int thickness, int lineType, int shift)}

  Draw an ellipse in the [center]() point, with its [axes]() (half of the size of the ellipse main axes), rotation
  [angle]() in degrees, [startAngle]() and [endAngle]() of the elliptic arc in degrees and [color](). For the lineType and
  shift see the line method.

  See also [MatDraw::line]()

{qmlBrief:Draws an ellipse.}

  Parameters :
  * [center]()
  * [axes]()
  * [angle]()
  * [startAngle]()
  * [endAngle]()
  * [color]()
  * [thickness]()
  * [lineType]()
  * [shift]()

{qmlMethod:MatDraw fillPoly(variant points, Color color, int lineType, int shift, Point offset)}

  Fills the area bounded by one or more polygons. The [points]() is the array of polygons where each polygon is
  represented by an array of points, the [color]() is the polygons color, the [lineType] is the type of the polygon
  boundaries and the shift is the number of fractional bits in the vertex coordinates. The offset is an optional offset
  of all points of the contours.

  See also [MatDraw::line]()

{qmlBrief:Fills an area bounded by one or more polygons.}

  Parameters :
  * [points]()
  * [color]()
  * [lineType]()
  * [shift]()
  * [offset]()

 














