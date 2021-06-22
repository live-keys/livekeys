# Reading Files

{using:resources}
{livekeysInit:open/resources/ShapeReadingFiles.qml}

This sample searches for a portion of text in a file, then displays the line
containing that portion of text.

A set of components are connected between each other to do the processing.
The connections are best seen in the [node editing palette]({livekeys-hover:livekeys://open/resources/ReadingFilesHighlight.qml#node-palette;livekeys://open/resources/ReadingFilesRemoveHighlight.qml#node-palette}). Below the palette, we can see each
component individually and its properties:

 * [`FileReader`]({livekeys-hover:livekeys://open/resources/ReadingFilesHighlight.qml#filereader;livekeys://open/resources/ReadingFilesRemoveHighlight.qml#filereader}) reads text from an input file, and stores it in its `data` property.

 * [`TextSearch`]({livekeys-hover:livekeys://open/resources/ReadingFilesHighlight.qml#textsearch;livekeys://open/resources/ReadingFilesRemoveHighlight.qml#textsearch}) will search for a [portion of text]({livekeys-hover:livekeys://open/resources/ReadingFilesHighlight.qml#search-value;livekeys://open/resources/ReadingFilesRemoveHighlight.qml#search-value}) in a larger text - in this case, the [text]({livekeys-hover:livekeys://open/resources/ReadingFilesHighlight.qml#read-text;livekeys://open/resources/ReadingFilesRemoveHighlight.qml#read-text}) that was read by the `FileReader`. The result will
 be a list of positions where the value was found in the text.

 * [`IndexSelector`]({livekeys-hover:livekeys://open/resources/ReadingFilesHighlight.qml#indexselector;livekeys://open/resources/ReadingFilesRemoveHighlight.qml#indexselector}) will select a single value from a list. We only want to select the first occurence of our portion of text, therefore
 we will use this component to select the first position where our portion of text was found.
 
* [`ConvertToInt`]({livekeys-hover:livekeys://open/resources/ReadingFilesHighlight.qml#converttoint;livekeys://open/resources/ReadingFilesRemoveHighlight.qml#converttoint}) is a check to make sure the value given by the index selector is a number.

* [`TextLineAtPosition`]({livekeys-hover:livekeys://open/resources/ReadingFilesHighlight.qml#textlineatposition;livekeys://open/resources/ReadingFilesRemoveHighlight.qml#textlineatposition}) will select the line of text at a given position within a larger text. We use this to get the line
of text where our portion of text was found. This component will give us the start and end positions of that line.

* [`TextClip`]({livekeys-hover:livekeys://open/resources/ReadingFilesHighlight.qml#textclip;livekeys://open/resources/ReadingFilesRemoveHighlight.qml#textclip}) will copy a portion of text from a [larger text]({livekeys-hover:livekeys://open/resources/ReadingFilesHighlight.qml#read-text-clip;livekeys://open/resources/ReadingFilesRemoveHighlight.qml#read-text-clip}), given the start and end positions. The positions selected by
`TextLineAtPosition` are [passed]({livekeys-hover:livekeys://open/resources/ReadingFilesHighlight.qml#positions;livekeys://open/resources/ReadingFilesRemoveHighlight.qml#positions}) to this component, giving us the actual line that contains the portion of text we were
searching for.

* The [`Text`]({livekeys-hover:livekeys://open/resources/ReadingFilesHighlight.qml#text-item;livekeys://open/resources/ReadingFilesRemoveHighlight.qml#text-item}) component just displays the found text in the [Application view pane]({livekeys-hover:livekeys://open/resources/ReadingFilesHighlight.qml#result;livekeys://open/resources/ReadingFilesRemoveHighlight.qml#result}).
