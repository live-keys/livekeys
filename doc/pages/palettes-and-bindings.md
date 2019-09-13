# Palettes and Bindings

Palettes and bindings are features in Livekeys that offer a new way to experience live coding. Before we describe them,
there's an intermediate feature in Livekeys we will look into that makes both of them possible.

## Direct Editing

Direct editing is the process of changing a property's value from code, without having to recompile the program. To see an
example, we can open up the sample project in _samples/live/palettesandbinding.qml_. Click on the rectangle to start
its animation, then right click on the `y` property from the main rectangle, then click on edit, and start editing the
value (the value will be highlighted in blue). After entering a new value, press CTRL+RETURN to commit the change. You
will see the rectangle has changed its position **without stopping the animation**. Behind the scenes, Livekeys tries
to find the element of the property that is changed. If it succeds, it will allow us to edit the value. After editing,
it will parse the value, and convert it from code to an actual value in our application.
This only works on values that can be parsed and converted from code (strings, numbers, etc).
Some object types have custom converters, but it's important to note that not all types can be directly edited.


## Palettes

On top of direct editing, palettes are visual items that edit the code through their controls. If a property's type is
convertable to a value, and there is a palette available for it, then the _Adjust_ entry in the context menu will be
activated. To open up a palette in our example, right click on the `y` property and click adjust. This will open up
our `double` editing palette. You can use the sliders in the palette to adjust the `y` property. You can try
the `color` property as well, which will open up a `color` editing palette.

## Runtime Bindings

Since direct editing modifies the value from code to our application, bindings do the reverse. Bindings
will listen for changes on a value, and when a change is detected, the code will be updated with that value. Just like
_editing_ and _palettes_ , bindings are activated in a property's context menu, using the _Bind_ entry.
You can use both palettes and bindings together to animate the sliders:


Note that bindings are broken if the value is changed manually or if the user clicks the _Unbind_ entry.
