# Static Items

In terms of live coding, whenever an application code changes, a new compilation is triggered, replacing
all the items within the previous build with a new set corresponding to the modified code. This means that each time
the user modifies something within the document, every object that was modified so far during execution will be
reset to its initial value. Unfortunately in some cases this is not the ideal scenario. Take a video player for example.
After creating the video player and the video starts playing, the user would like to add a set of filters to the video
in order to adjust its brightness and contrast. Now, whenever a new filter is added, the video starts playing from the
beginning, making it unsuitable to adjust the filters values. In order to avoid these scenarios, Live CV introduces
the concept of static items. As a definition, in Live CV, static items are items that keep their state between multiple
compilations. They are recognized by requiring the use of the `staticLoad` function. Take the VideoCapture
for example:

```
VideoCapture{
    id : videoArea
    loop : true
    fps : 100
    Component.onCompleted : staticOpen(videoPath)
}
```

The `VideoCapture` works by keeping its state (in this case the progress made on the video) outside the
actual built application. Whenever it's created, it will load or create its state (depending on whether the state was
already created from previous applications) from a specified location. The location is usually specified by an
identifier, in this case the identifier is equal to the video file path.

It's important to note that a state is kept alive between multiple compilations as long as the state is being used in
each successful consecutive compilation. (If the compilations contain errors, then the state cleaning process does not
get triggered). If we were to delete the `VideoCapture` or switch it to a different file, then the state
associated with the previous file would not be used, making Live CV take note of that and clearing the unused space.
So if we were to come back to the same file, then the progress made previously would be lost again.

In terms of importance, static items can be used in all kinds of scenarios where the user would like to keep a
consistent item state for multiple compilations, like algorithms that require training (Background extraction, object
detection) or in places where items require a long time to load (3d objects, downloadable content) or simply
modifications during runtime that require consistency.

## StaticLoader

The `StaticLoader` is an item part of [live](live) plugin similar to the Loader item in QtQuick, the difference
being that its items are loaded statically. To do this, its item is registered as a state that can be retrieved when
the `StaticLoader` is created. The [StaticLoader](StaticLoader) sample shows the usage of this item. The color property,
which is part of the StaticLoaders state gets modified once the user clicks on the color, and will keep its color
during changes to the code. Note that **any changes to the StaticLoader's contents will not take effect while the
state identifier is the same**. For example, if we want to change the color from code, or want to add another property
to the StaticLoaders contents, we need to also change its identifier, since Live CV will not replace the contents of
our state as long as the state is active.

## Nesting Static Items

Consider the following qml component:

```
// Interact.qml

import QtQuick 2.3
import live 1.0

Rectangle{
    width : 100
    height : 100

    StaticLoader{
        id : staticLoader
        source : Rectangle{ color: "#ff0000" }
        Component.onCompleted : { staticLoad("item1") }
    }

    Rectangle{
        anchors.fill: parent
        color : staticLoader.item ? staticLoader.item.color: "transparent"
        MouseArea{
            anchors.fill : parent
            onClicked : {
                staticLoader.item.color = "#ff00ff"
            }
        }
    }
}
```

If we want to use this component in our application in more than one place, we're going to end up with some weird
behavior:

```
import QtQuick 2.3

Row{
    Interact{}
    Interact{}
}
```

When we click on the first component, you will notice that both components will change their color. The reason why this
is happening is that **both components have StaticLoaders that load their items from the same place**. It's important to
be careful when a component is using any static item, be it a video capture or a background subtractor, to forward their
call to the `staticload` function:

```
// Interact.qml

import QtQuick 2.3
import live 1.0

Rectangle{
    width : 100
    height : 100

    function staticLoad(key){
        staticLoader.staticLoad(key)
    }

    StaticLoader{
        id : staticLoader
        source : Rectangle{ color: "#ff0000" }
    }

    Rectangle{
        anchors.fill: parent
        color : staticLoader.item ? staticLoader.item.color: "transparent"
        MouseArea{
            anchors.fill : parent
            onClicked : {
                staticLoader.item.color = "#ff00ff"
            }
        }
    }
}
```

Then when the user uses the component, he is required to call the staticLoad in order to use it:

```
import QtQuick 2.3

Row{
    Interact{
        Component.onCompleted : staticLoad('item1')
    }
    Interact{
        Component.onCompleted : staticLoad('item2')
    }
}
```

Another way to define this is that \b{any component that nests a static component will itself become static}.
