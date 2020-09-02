# Linking Executables

{using:resources}
{livekeysInit:open/resources/ShapeLinkingExecutablesWin.qml}

This example shows how Livekeys can run external applications and scripts.

Livekeys can connect to their input/output streams in order to process or display their data.

Livekeys can also connect scripts between each other, similar to linux pipes, where the output of one script
becomes the input of the other.

In this example, we are executing 2 external commands, as you can see in the (node editor)[]. The first one
lists files in the current directory, and the second takes in the list and counts the number of lines produced.

The application being run is set through the (path)[] property. Arguments are sent via a list.
In and out streams are connected in the node editor.


The third component is a simple (StreamLog)[], logging the output of the second component into Livekeys. You
can view it in the (log pane)[].






