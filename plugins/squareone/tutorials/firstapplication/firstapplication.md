# First application

{using:resources}
{livekeysInit:open/resources/ShapeSwitch.qml}

In comparison to the previous tutorial, here here we have 4 panes open:

 * [the project pane]({livekeys-hover:livekeys://open/resources/FirstApplicationHighlight.qml#project-pane;livekeys://open/resources/FirstApplicationRemoveHighlight.qml#project-pane}), showing the files in the project

 * [the documentation pane]({livekeys-hover:livekeys://open/resources/FirstApplicationHighlight.qml#documentation-pane;livekeys://open/resources/FirstApplicationRemoveHighlight.qml#documentation-pane}), or this page, and

 * [the editor pane]({livekeys-hover:livekeys://open/resources/FirstApplicationHighlight.qml#editor-pane;livekeys://open/resources/FirstApplicationRemoveHighlight.qml#editor-pane}), currently displaying the contents of the active file

 * [the view]({livekeys-hover:livekeys://open/resources/FirstApplicationHighlight.qml#view-pane;livekeys://open/resources/FirstApplicationRemoveHighlight.qml#view-pane}), where the
application we build is displayed.

## The editor pane

The editor pane contains the main file, the file that's being run by livekeys, and also the one we will be editing.

In the editor, there are 2 sections:

 * [The imports section]({livekeys-hover:livekeys://open/resources/FirstApplicationHighlight.qml#imports-section;livekeys://open/resources/FirstApplicationRemoveHighlight.qml#imports-section})

 * [The items section]({livekeys-hover:livekeys://open/resources/FirstApplicationHighlight.qml#items-section;livekeys://open/resources/FirstApplicationRemoveHighlight.qml#items-section})

The imports section is used to add plugins to the document. Each plugin brings new features, `QtQuick` for
example comes with user interface support, and `lcvcore` adds components for photo and video editing.

The items section describes the application that's being build. The application is made out of a series of items, grouped in a hierarchy,
with a single item at the top of the hierarchy, or the [root item]({livekeys-hover:livekeys://open/resources/FirstApplicationHighlight.qml#root-item;livekeys://open/resources/FirstApplicationRemoveHighlight.qml#root-item}), under which other
items can be added, each of which comes with it's own functionality.

Let's try some new items.

## Adding new items

Next to the root item, there's a set of
[actions]({livekeys-hover:livekeys://open/resources/FirstApplicationHighlight.qml#item-options;livekeys://open/resources/FirstApplicationRemoveHighlight.qml#item-options})
we can trigger for that item, The one we are interested in is the
[add action]({livekeys-hover:livekeys://open/resources/FirstApplicationHighlight.qml#add-option;livekeys://open/resources/FirstApplicationRemoveHighlight.qml#add-option}),
where we can add properties / events or other items inside the selected one.

 * Click on the [add icon]({livekeys-hover:livekeys://open/resources/FirstApplicationHighlight.qml#add-option;livekeys://open/resources/FirstApplicationRemoveHighlight.qml#add-option}) option

 * Select [Object]({livekeys-hover:livekeys://open/resources/FirstApplicationHighlight.qml#add-option-object;livekeys://open/resources/FirstApplicationRemoveHighlight.qml#add-option-object})

 * Then, use [the search]({livekeys-hover:livekeys://open/resources/FirstApplicationHighlight.qml#add-option-search;livekeys://open/resources/FirstApplicationRemoveHighlight.qml#add-option-search})
to find the `VideoDecoderView`, then double click to add it. You can now see it added to our document.

 * Now, we need to select a video to play. Open a video by opening the
 [path property]({livekeys-hover:livekeys://open/resources/FirstApplicationHighlight.qml#video-path;livekeys://open/resources/FirstApplicationRemoveHighlight.qml#video-path}).
You can also just add a default video by [clicking here](livekeys://open/resources/AddDefaultVideo.qml).

## Connecting items

The VideoDecoderView already started playing our video. Now we can connect other items to the VideoDecoderView to do different forms of processing.
Let's try to add a conversion to grayscale to our video. Within the root item, click the
['add']({livekeys-hover:livekeys://open/resources/FirstApplicationHighlight.qml#add-option;livekeys://open/resources/FirstApplicationRemoveHighlight.qml#add-option})
option, select [Object]({livekeys-hover:livekeys://open/resources/FirstApplicationHighlight.qml#add-option-object;livekeys://open/resources/FirstApplicationRemoveHighlight.qml#add-option-object})
then search and add the `GrayscaleView` item. Once added, in the input property, type
'videoDecoderView.image', then hit the return key or click on the update button. You should see the grayscale version of our video playing.

## Looking over the hierarchy

Applications in Livekeys are build from a set of items. Each of them has its own set of properties we can configure. (i.e.
`VideoDecoderView` has the path property that sets the path to a video.) Properties can be connected with each other, so
that the generated output of one property becomes the input of the other, like we did between the `VideoDecoderView image` and
the `GrayScaleView input`. These combinations become a powerful mechanism for automation, as we will see in tutorials to
come.



