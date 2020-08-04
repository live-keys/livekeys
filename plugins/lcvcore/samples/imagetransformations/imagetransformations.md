# Image Transformations

{using:resources}
{livekeysInit:open/resources/ShapeSwitch.qml}

In this sample, we can add a series of image transformations using a single
transformations object. The object will create a pipeline through
which the image will pass in order to get our output.

Try for example to crop the image using the crop tool. You will notice the transformation
added to our list.

Switching to a new image won't reset our transformations, but rather will apply them
to the new image. You can use such transformations and apply them to a series of images
automatically.

