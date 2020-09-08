# Color Adjustments

{using:resources}
{livekeysInit:open/resources/ShapeColorAdjustments.qml}


This sample has a pipeline set up to process the colors of an image:

 * The image is first [loaded]({livekeys-hover:livekeys://open/resources/ColorAdjustmentsHighlight.qml#loading;livekeys://open/resources/ColorAdjustmentsRemoveHighlight.qml#loading}) from a file

 * A [levels]({livekeys-hover:livekeys://open/resources/ColorAdjustmentsHighlight.qml#levels;livekeys://open/resources/ColorAdjustmentsRemoveHighlight.qml#levels})  component can adjust the brightness, contrast and tonal range of the image.

 * Then, the output of that is sent through a [hue, saturation and lightness filter]({livekeys-hover:livekeys://open/resources/ColorAdjustmentsHighlight.qml#hsl;livekeys://open/resources/ColorAdjustmentsRemoveHighlight.qml#hsl}).

 * And lastly, there's a simple component to adjust the [brightness and contrast]({livekeys-hover:livekeys://open/resources/ColorAdjustmentsHighlight.qml#brightness-and-contrast;livekeys://open/resources/ColorAdjustmentsRemoveHighlight.qml#brightness-and-contrast}).

 * The [ImageView]({livekeys-hover:livekeys://open/resources/ColorAdjustmentsHighlight.qml#imageview;livekeys://open/resources/ColorAdjustmentsRemoveHighlight.qml#imageview}) displays the ImageResult in our application on the right.

The same pipeline as above is used in the [Video Color Adjustments](livekeys://open-project/lcvcore/samples/videocoloradjustments) sample, showing how
easy it is in Livekeys to switch between an image and video with the same settings.
