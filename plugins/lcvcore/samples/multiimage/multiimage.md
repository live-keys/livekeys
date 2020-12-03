# Color Adjustments on Multiple Images

{using:resources}
{livekeysInit:open/resources/ShapeMultiImage.qml}

This sample uses the pipeline described in the Color Adjustments sample and applies it to multiple images.

There are 2 documents opened, multiimage.qml, and Operations.qml.

In *multiimage.qml*:

 * We create a list of paths where we want to load the images from

 * We set up the list of paths as a model for a ListView. The ListView simply displays a set of items, or
 `delegates` where each `delegate` is created for each item in the model. As our model has 3 items, we will
 have 3 delegates created.

 * The `delegate` is a `Component` that has a `Builder`, which will build the items in *Operations.qml*.
 In this case, for the 3 paths given in the list, there will be 3 items (one for each path), created
 from *Operations.qml*.

*Operations.qml*:

 * Contains the same components as the color adjustments sample.

 * Instead of providing the actual path of the image, the value of `modelData` is used. The `modelData`
 is assigned automatically when the Builder creates this item from ListView.

 * Has a builder icon, which allows you to rebuild the sample in the `Builder` from `multiimage.qml`.
 Whenever you make a change to this file, the change won't trigger immediately, as the builder needs
 to reapply these changes to all images. Whether the right builder is selected can be checked from
 the Connections icon.

 * To see the actions apply, change the hue of the image, and then trigger the builder from the builder icon.
 You will see the new hue value being applied to all images.
