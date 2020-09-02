# Drawing pipeline

{using:resources}
{livekeysInit:open/resources/ShapeDrawing.qml}

This sample shows a processing pipeline for drawing.

 * The (first component)[] creates a blank white image of size 800x600

 * Our second component is a draw component, allowing you to draw on the blank image.
 You can select the brush size by (this icon)[], and the color through the (box just
 below)[].

 * The third component is a color histogram of the image, displayed (here)[], that
 will update in real-time as the image is being drawn on.

This sample shows how users can configure the post processing of an
image that's being drawn on live. You can augment the drawing by applying sets of filters
to the image, and then watch them update through the drawing process.
