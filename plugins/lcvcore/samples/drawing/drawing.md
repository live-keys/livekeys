# Drawing pipeline

{using:resources}
{livekeysInit:open/resources/ShapeDrawing.qml}

This sample shows a processing pipeline for drawing.

 * The [first component]({livekeys-hover:livekeys://open/resources/DrawingHighlight.qml#first-component;livekeys://open/resources/DrawingRemoveHighlight.qml#first-component}) creates a blank white image of size 800x600

 * Our second component is a [`DrawSurface` component]({livekeys-hover:livekeys://open/resources/DrawingHighlight.qml#draw-component;livekeys://open/resources/DrawingRemoveHighlight.qml#draw-component}), allowing you to draw on the blank image, or create a gradient.
 You can select the brush tool by clicking on [this icon]({livekeys-hover:livekeys://open/resources/DrawingHighlight.qml#brush-icon;livekeys://open/resources/DrawingRemoveHighlight.qml#brush-icon}). After that, you can change the brush size by clicking [here]({livekeys-hover:livekeys://open/resources/DrawingHighlight.qml#brush-size;livekeys://open/resources/DrawingRemoveHighlight.qml#brush-size}) and dragging the slider. 
 The gradient tool can be selected by clicking on [this icon]({livekeys-hover:livekeys://open/resources/DrawingHighlight.qml#gradient-icon;livekeys://open/resources/DrawingRemoveHighlight.qml#gradient-icon}), and the gradient itself is created by dragging a point across the image shown. 
 For both tools, you can select the foreground and background colors with the pair of [color pickers]({livekeys-hover:livekeys://open/resources/DrawingHighlight.qml#color-pickers;livekeys://open/resources/DrawingRemoveHighlight.qml#color-pickers}).

 * The [third component]({livekeys-hover:livekeys://open/resources/DrawingHighlight.qml#histogram-component;livekeys://open/resources/DrawingRemoveHighlight.qml#histogram-component}) is a color histogram of the image, displayed [here]({livekeys-hover:livekeys://open/resources/DrawingHighlight.qml#histogram-view;livekeys://open/resources/DrawingRemoveHighlight.qml#histogram-view}), that
 will update in real-time as the image is being drawn on.

This sample shows how users can configure the post processing of an
image that's being drawn on live. You can augment the drawing by applying sets of filters
to the image, and then watch them update through the drawing process.
