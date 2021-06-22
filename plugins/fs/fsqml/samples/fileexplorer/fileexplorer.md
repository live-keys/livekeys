# File Explorer

{using:resources}
{livekeysInit:open/resources/ShapeFileExplorer.qml}

This sample shows how you can apply different image filters at once, by simply
passing over a set of files.

The [file explorer]({livekeys-hover:livekeys://open/resources/FileExplorerHighlight.qml#fileexplorer;livekeys://open/resources/FileExplorerRemoveHighlight.qml#fileexplorer}) component allows you to navigate accross files within your hard drive.
You can select a file by double-clicking on it.

If the file selected is an image (ends with the jpg or png extension), then the
[`ExtensionPass`]({livekeys-hover:livekeys://open/resources/FileExplorerHighlight.qml#extensionpass;livekeys://open/resources/FileExplorerRemoveHighlight.qml#extensionpass}) will forward it to the [`ImageRead`]({livekeys-hover:livekeys://open/resources/FileExplorerHighlight.qml#imageread;livekeys://open/resources/FileExplorerRemoveHighlight.qml#imageread}) component, where the image will
be read.

Then, the image is distributed to a [`Sepia`]({livekeys-hover:livekeys://open/resources/FileExplorerHighlight.qml#sepia;livekeys://open/resources/FileExplorerRemoveHighlight.qml#sepia}) filter, a [`Temperature`]({livekeys-hover:livekeys://open/resources/FileExplorerHighlight.qml#temperature-1;livekeys://open/resources/FileExplorerRemoveHighlight.qml#temperature-1}) filter with a warming effect,
and a [`Temperature`]({livekeys-hover:livekeys://open/resources/FileExplorerHighlight.qml#temperature-2;livekeys://open/resources/FileExplorerRemoveHighlight.qml#temperature-2}) filter with a cooling effect.

You can use this as a quick way to transition between images and test out different effects
on those images.
