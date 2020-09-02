# File Explorer

{using:resources}
{livekeysInit:open/resources/ShapeFileExplorer.qml}

This sample shows how you can apply different image filters at once, by simply
passing over a set of files.

The file explorer component allows you to navigate accross files within your harddrive.
You can select a file by double clicking on it.

If the file selected is an image (ends with the jpg or png extension), then the
ExtensionPass will forward it to the ImageRead component, where the image will
be read.

Then, the image is distributed to a Sepia filter, a Temperature filter with a warming effect,
and a Temperature filter with a cooling effect.

You can use this as a quick way to transition between images and test out different effects
on those images.
