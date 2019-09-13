# Building Livekeys


All modules for Livekeys are available through qmake projects. To compile Livekeys you need to have Qt 5.6 or higher, and
Open CV 3.1 or higher. On windows, OPENCV_DIR
[environment variable](http://docs.opencv.org/2.4/doc/tutorials/introduction/windows_install/windows_install.html)
needs to be set, or you can configure the variable whithin the source code of Livekeys in
**projects/3rdparty/opencv.pri** before compiling.

You can build the application either by opening **livekeys.pro** in QtCreator and building from there, or by manually
running qmake in your build directory:

```
cd build
qmake -r ..
make (or nmake)
```

## Modules

Livekeys is split into 3 main modules:

 * **The application**, containing the visual elements and the runtime types
 * **The editor**, containing components for projects and editing
 * **The plugins**, containing the plugins you can import, some of which can also be linked to as libraries in ordeer
to provide common transferable types.

Further down, the editor contains the [lcveditor_cpp](lcveditor-cpp.md) module, which you can link to in order to
provide editor functionality to Livekeys(e.g. palettes, code completion, highlighting for different file types)

The plugins module then contains the [live](plugin-live.md) plugin, which offers components for live coding
and communication with Livekeyss interface, and lcvcore library which offers computer vision specific main types (mostly
named after Open CV). You link to them as libraries in order to create Livekeys specific plugins.

It's also important to note that Livekeys as an application is mostly plugin based, so we can for example disable all
Open CV based plugins from our build configuration, and build Livekeys outside of Open CV dependencies. All QtQuick based
plugins are compatible by default, and you can build any plugins derived from those without having to link to any Livekeys
libraries.

