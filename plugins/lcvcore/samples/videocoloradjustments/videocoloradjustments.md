# Video Color Adjustments

{using:resources}
{livekeysInit:open/resources/ShapeVideoColorAdjustments.qml}

This sample has a pipeline set up to process the colors of video frames. It uses the same
components as the [Color Adjustments](livekeys://open-project/lcvcore/samples/coloradjustments) sample, where an image instead
of a video is used. The [`VideoDecoderView`]({livekeys-hover:livekeys://open/resources/VideoColorAdjustmentsHighlight.qml#video-decoder-view;livekeys://open/resources/VideoColorAdjustmentsRemoveHighlight.qml#video-decoder-view}) plays a video from a given [file]({livekeys-hover:livekeys://open/resources/VideoColorAdjustmentsHighlight.qml#file;livekeys://open/resources/VideoColorAdjustmentsRemoveHighlight.qml#file}), and shows the
frames received on the right side of our application. Then, the frame gets pased through the
[`Levels`]({livekeys-hover:livekeys://open/resources/VideoColorAdjustmentsHighlight.qml#levels;livekeys://open/resources/VideoColorAdjustmentsRemoveHighlight.qml#levels}), [`HueSaturationLightness`]({livekeys-hover:livekeys://open/resources/VideoColorAdjustmentsHighlight.qml#hsl;livekeys://open/resources/VideoColorAdjustmentsRemoveHighlight.qml#hsl}) and [`BrightnessAndContrast`]({livekeys-hover:livekeys://open/resources/VideoColorAdjustmentsHighlight.qml#bac;livekeys://open/resources/VideoColorAdjustmentsRemoveHighlight.qml#bac}) components.
You can try out the sliders to see the result.

You can also view the image at any intermediate stage, by opening an image palette at
any one of the input properties of each component. You can also save the frame, by clicking
on the [`save` icon]({livekeys-hover:livekeys://open/resources/VideoColorAdjustmentsHighlight.qml#save-button;livekeys://open/resources/VideoColorAdjustmentsRemoveHighlight.qml#save-button}) at the top left of each image palette.

