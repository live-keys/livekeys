# PaperGrapherQml

PaperGrapherQml is a vector editing application in qml.

It's a port of *papergrapher* to qml (see [papergrapher](https://github.com/w00dn/papergrapher)). 

Internally it uses [paperqml](https://github.com/dinusv/paperqml) for the graphics part.

## Build

To build, simply run qmake on `papergrapher.pro`:

```
qmake -r .
make (or nmake)
```

## Features

The application provides full support for vector graphics:

 * Shapes
 * Paths
 * Fonts / font to paths
 * Layers
 * Undo/redo
 * Saving/opening new documents
