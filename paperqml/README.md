# PaperQml

PaperQml is a port of *paper.js* to qml (see [paper.js](http://paperjs.org)). 

PaperQml adds vector graphics scripting to qml.

## Installation

To install, simply clone the repo, and compile the sample in `samples/paperqml.pro`:

```
cd samples
qmake -r .
make (or nmake)
```

## Usage

The sample in `samples/main.qml` shows how to draw simple paths, incldue mouse interactions 
and add animations.

PaperQml wraps everything around a `PaperCanvas` item:

```js
PaperCanvas{
    id: paperCanvas
    width: 300
    height: 300

    onPaperReady: { // paper.js functions
        var path = new paper.Path()
        path.strokeColor = '#ff0000'
        var start = new paper.Point(100, 100)
        path.moveTo(start);
        path.lineTo(start.add([ 200, -50 ]));
        paperCanvas.paint()
    }
}
```

`PaperCanvas` also provides a qml Timer internally, so adding animations will work 
out of the box:

```js
PaperCanvas{
    id: paperCanvas3
    width: 300
    height: 300

    onPaperReady: {
        var path = new paper.Path.Rectangle({
            point: [75, 75],
            size: [75, 75],
            strokeColor: 'black'
        });

        paper.view.onFrame = function(event){
            path.rotate(3);
        }
    }
}
```

## Including PaperQml in other projects

To include PaperQml in your project, you will need to:

 * Compile the `JsModule` class found in the `src` folder.

 * Set it as part of the root context in `jsModule` when starting the application:

```cpp
QQmlApplicationEngine engine;
engine.rootContext()->setContextProperty("jsModule", JsModule::provider(&engine, nullptr));
```

 * Include and make `qml/PaperCanvas.qml` available in your project
 * Include `js/paper-dom.js`, `js/paper-full.js`, `js/xmldom.js` in your project. 
 When loading these files, `PaperCanvas` searches for them in
the main resource file of the application. In case the files are copied to another path,
they can be configured as such:

```js
PaperCanvas{
    id: paperCanvas
    width: 300
    height: 300
    xmldomPath: 'path/to/xmldom.js'
    domScriptPath: 'path/to/paper-dom.js'
    paperScriptPath: 'path/to/paper-full.js'
}
```

## Implementation Info

Since qml's javascript engine does not properly support *requiring* javascript files,
PaperQml uses the distributed `paper-full.js` file.

The `js/paper-dom.js` file provides minimal DOM functionality to make paperjs work.

`PaperCanvas` wraps a qml Canvas object, and loads both `paper-dom` and `paper-full` and
emits the `paperReady` signal once both files have finished loading.

## Limitations

### Opacity and blend modes

In comparison to the html Canvas, the qml Canvas allows access to its context only during the `onPaint`
handler:

```js
Canvas{
    onPaint: {
        var ctx = getContext("2d");
        // use ctx ...
    }
}
```

This disables the application to create and work with multiple Canvases at the same time, as the 
application needs to wait for each Canvas to trigger the `onPaint` handler.

Paper.js uses this functionality in `CanvasProvider`, where it creates and reuses canvases for 
a number of reasons, including opacity and blending of items. Due to the above constraints, 
this implementation forces `CanvasProvider` to work with a single canvas, so features 
like opacity and blend modes will not work.

### Speed

This implementation runs slightly slower than the browser alternative.



