# Drawing pipeline

{using:resources}
{livekeysInit:open/resources/ShapeDrawing.qml}

This sample shows a processing pipeline for drawing.

 * The [first component]({livekeys-hover:livekeys://open/resources/DrawingHighlight.qml#first-component;livekeys://open/resources/DrawingRemoveHighlight.qml#first-component}) creates a blank white image of size 800x600

 * Our second component is a [`Draw` component]({livekeys-hover:livekeys://open/resources/DrawingHighlight.qml#draw-component;livekeys://open/resources/DrawingRemoveHighlight.qml#draw-component}), allowing you to draw on the blank image.
 You can select the brush size by [this icon]({livekeys-hover:livekeys://open/resources/DrawingHighlight.qml#brush-icon;livekeys://open/resources/DrawingRemoveHighlight.qml#brush-icon}), and the color through the [box just
 below]({livekeys-hover:livekeys://open/resources/DrawingHighlight.qml#color-icon;livekeys://open/resources/DrawingRemoveHighlight.qml#color-icon}).

 * The [third component]({livekeys-hover:livekeys://open/resources/DrawingHighlight.qml#histogram-component;livekeys://open/resources/DrawingRemoveHighlight.qml#histogram-component}) is a color histogram of the image, displayed [here]({livekeys-hover:livekeys://open/resources/DrawingHighlight.qml#histogram-view;livekeys://open/resources/DrawingRemoveHighlight.qml#histogram-view}), that
 will update in real-time as the image is being drawn on.

This sample shows how users can configure the post processing of an
image that's being drawn on live. You can augment the drawing by applying sets of filters
to the image, and then watch them update through the drawing process.
