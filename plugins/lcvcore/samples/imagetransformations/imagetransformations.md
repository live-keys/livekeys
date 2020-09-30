# Image Transformations

{using:resources}
{livekeysInit:open/resources/ShapeImageTransformations.qml}

In this sample, we can add a series of image transformations using a single
transformations object. The object will create a pipeline through
which the image will pass in order to get our output.

Currently you can [crop]({livekeys-hover:livekeys://open/resources/ImageTransformationsHighlight.qml#crop-button;livekeys://open/resources/ImageTransformationsRemoveHighlight.qml#crop-button}), [resize]({livekeys-hover:livekeys://open/resources/ImageTransformationsHighlight.qml#resize-button;livekeys://open/resources/ImageTransformationsRemoveHighlight.qml#resize-button}) or [rotate]({livekeys-hover:livekeys://open/resources/ImageTransformationsHighlight.qml#rotate-button;livekeys://open/resources/ImageTransformationsRemoveHighlight.qml#rotate-button}) the image. Try for example to crop the image using the crop tool. You will notice the [transformation]({livekeys-hover:livekeys://open/resources/ImageTransformationsHighlight.qml#transformation;livekeys://open/resources/ImageTransformationsRemoveHighlight.qml#transformation})
added to our list.

[Switching to a new image](livekeys://open/resources/AddDifferentImage.qml) won't reset our transformations, but rather will apply them
to the new image. The transformations you add will be saved in code, and can be applied
to other images that pass through.

