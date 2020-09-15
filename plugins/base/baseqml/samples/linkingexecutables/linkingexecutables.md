# Linking Executables

{using:resources}
{livekeysInit:open/resources/ShapeLinkingExecutablesWin.qml}

This example shows how Livekeys can run external applications and scripts.

Livekeys can connect to their input/output streams in order to process or display their data.

Livekeys can also connect scripts between each other, similar to Linux pipes, where the output of one script
becomes the input of the other.

In this example, we are executing 2 external commands, as you can see in the [node editor]({livekeys-hover:livekeys://open/resources/LinkingExecutablesHighlight.qml#node-palette;livekeys://open/resources/LinkingExecutablesAdjustmentsRemoveHighlight.qml#node-palette}). 
The [first]({livekeys-hover:livekeys://open/resources/LinkingExecutablesHighlight.qml#first-exec;livekeys://open/resources/LinkingExecutablesAdjustmentsRemoveHighlight.qml#first-exec}) one
lists files in the current directory, and the [second]({livekeys-hover:livekeys://open/resources/LinkingExecutablesHighlight.qml#second-exec;livekeys://open/resources/LinkingExecutablesAdjustmentsRemoveHighlight.qml#second-exec}) takes in the list and counts the number of lines produced.

The application being run is set through the [path]({livekeys-hover:livekeys://open/resources/LinkingExecutablesHighlight.qml#path;livekeys://open/resources/LinkingExecutablesAdjustmentsRemoveHighlight.qml#path}) property. Arguments are sent via a [list]({livekeys-hover:livekeys://open/resources/LinkingExecutablesHighlight.qml#arg-list;livekeys://open/resources/LinkingExecutablesAdjustmentsRemoveHighlight.qml#arg-list}).
In and out streams are [connected]({livekeys-hover:livekeys://open/resources/LinkingExecutablesHighlight.qml#connection;livekeys://open/resources/LinkingExecutablesAdjustmentsRemoveHighlight.qml#connection}) in the node editor.

The third component is a simple [StreamLog]({livekeys-hover:livekeys://open/resources/LinkingExecutablesHighlight.qml#streamlog;livekeys://open/resources/LinkingExecutablesAdjustmentsRemoveHighlight.qml#streamlog}), logging the output of the second component into Livekeys. You
can view it by opening the [log pane]({livekeys-hover:livekeys://open/resources/LinkingExecutablesHighlight.qml#log-view;livekeys://open/resources/LinkingExecutablesAdjustmentsRemoveHighlight.qml#log-view}).






