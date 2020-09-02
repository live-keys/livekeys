# Video Color Adjustments

{using:resources}
{livekeysInit:open/resources/ShapeVideoColorAdjustments.qml}

This sample has a pipeline set up to process the colors of video frames. It uses the same
components as the (Color Adjustments)[color_adjustments] sample, where an image instead
of a video is used. The `VideoDecoderView` plays a video from a given file, and shows the
frames received on the right side of our application. Then, the frame gets pased through the
levels, hue-saturation-lightness and brightness-and-contrast components.
You can try out the sliders to see the result.

You can also view the image at any intermediate stage, by opening an image palette at
any one of the input properties of each component. You can also save the frame, by clicking
on the `save` icon at the top left of each ImageView palette.

