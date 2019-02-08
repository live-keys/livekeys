# Plugins

In Live CV, plugins are treated and imported the same way as in QtQuick, with one addition. In Live CV, plugins can have
external dependencies, as their paths are searched for before by Live CVs plugin loading process. This makes it easier to
write plugins that wrap external libraries (e.g. Open CV, Dlib), since the process of using them in Live CV is easier. While
QtQuick plugins work in Live CV out of the box, integrating Live CV plugins into QtQuick applications require the
user to make sure that all libraries are locatable by the application executable (e.g. to use lcvcore
opencv_*.dll / opencv_*.so have to be part of PATH or LD_LIBRARY_PATH environment variables).

Plugins can either be in binary form (written in c++), or as qml components. The plugins folder is used by default to
locate a plugin. (e.g. using \c{import opencv.contrib.core} will look into _<livecv>/plugins/opencv/contrib/core_ for the
specified import). All plugins need to have a _qmldir_ file in their root in order to be visible to the application.
[This article](http://doc.qt.io/qt-5/qtqml-modules-qmldir.html) describes the process of defining a
qmldir file. Qml plugins based only on qml components require you to only add the _qmldir_ file and list the qml
components you want to expose to the application. For example, you can go to _<livecv>/plugins_ and create a folder
named _example_, and add the following _qmldir_:

```
module example
HelloText 1.0 HelloText.qml
```

Then add the \e{HelloText.qml} item:

```
import QtQuick 2.3
Text{ text: "Hello"; color: "#f00" }
```

In Live CV then we can use our component to see the given text:

```
import example 1.0

HelloText{}
```

Creating c++ based plugins is described in the [developer section](Creating_a_Plugin).


## Packages

Packages in Live CV contain one or more plugins that provide a common functionality. Installation of packages
currently has to be done manually (a package manager is planned in the upcoming versions), so to install a package, copy
the contents of its _plugins_ folder into the _plugins_ folder of your livecv installation and the contents of
the _dependencies_ folder in case it exists into your live cv \e{dependencies} folder. It's best to restart Live CV in
order for the new components to be loaded correctly.