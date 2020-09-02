# Color Adjustments

{using:resources}
{livekeysInit:open/resources/ShapeColorAdjustments.qml}

This sample has a pipeline set up to process the colors of an image:

 * The image is first loaded from a file

 * A levels component can adjust the brightness, contrast
 and tonal range of the image.

 * Then, the output of that is sent through a hue, saturation and lightness filter.

 * And lastly, there's a simple component to adjust the brightness and contrast.

 * The ImageView displays the ImageResult in our application on the right.

The same pipeline as above is used in the (Video Color Adjustments)[video_color_adjustment] sample, showing how
easy it is in Livekeys to switch between an image and video with the same settings.
