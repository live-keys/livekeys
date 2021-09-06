# Projects

A Livekeys project consists of one or multiple Qml source files, which are interpreted and compiled by a qml engine. The qml language is developed
and maintained by Qt and comes with a ton of elements (as part of QtQuick module), which you can use within Livekeys.
You can find documentation and details on the language and it's elements on Qt's website. Apart from those, Livekeys
comes with its own set of plugins and elements, specialized mostly on live coding and computer vision, which we will
dive into later on.

Currently Livekeys is able to have a single project opened at once, and run a single main file (also called the active
file) within that project. There are 2 types of projects that can be opened: **file based** or **folder based**. File
based projects are basically single qml files that are opened and run directly as active files. Folder based projects
can have one or more qml files out of which one will be the active one. The active file can be changed by the user at
any time through the project view.

By default, when opening a project folder, Livekeys will automatically try to find the file to run whithin that project
and set it as active, using the following rules:

* Looking for a _main.qml_ file
* If there's no _main.qml_, then the looking for the first _.qml_ file starting with a lower case letter
* If no _.qml_ file starting with a lower case letter is found, then the first found _.qml_ file
* If the project has no _.qml_ files, then the project will be open in edit mode
