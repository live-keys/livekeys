# Reading Files

{using:resources}
{livekeysInit:open/resources/ShapeSwitch.qml}

This sample searches for a portion of text in a file, then displays the line
containing that portion of text.

A set of components are connected between each other to do the processing.
The connections are best seen in the node editing palette. Below the palette, we can see each
component individually and its properties:

 * FileReader reads text from an input file, and stores it in its `data` property.

 * TextSearch will search for a portion of text in a larger text, in this case, the text that was read by the FileReader, The result will
 be a list of positions where the value was found in the text.

 * IndexSelector will select a single value from a list. We only want to select the first occurence of our portion of text, therefore
 we will use this component to select the first position where our portion of text was found.

 * ConvertToInt is a check to make sure the value given by the index selector is a number.

 * TextLineAtPosition will select the line of text at a given position within a larger text. We use this to get the line
 of text where our portion of text was found. This component will give us the start and end positions of that line.

 * TextClip will copy a portion of text from a larger text, given the start and end positions. The positions selected by
 TextLineAtPosition are passed to this component, giving us the actual line that contains the portion of text we were
 searching for.

 * The Text component just displays the found text in the Application view pane.
