# Running Live CV

Once Live CV has installed, the following window should appear when running the application:

![](images/startscreen.jpg)

The view consists of 4 main components which we will treat in the following sections:

* Header
* Project View
* Editor
* Viewer

Before we go into each component, it's important to understand how Live CV projects work. A Live CV project consists
of one or multiple Qml source files, which are interpreted and compiled by a qml engine. The qml language is developed
and maintained by Qt and comes with a ton of elements (as part of QtQuick module), which you can use within Live CV.
You can find documentation and details on the language and it's elements on Qt's website. Apart from those, Live CV
comes with its own set of plugins and elements, specialized mostly on live coding and computer vision, which we will
dive into later on.

Currently Live CV is able to have a single project opened at once, and run a single main file (also called the active
file) within that project. There are 2 types of projects that can be opened: **file based** or **folder based**. File
based projects are basically single qml files that are opened and run directly as active files. Folder based projects
can have one or more qml files out of which one will be the active one. The active file can be changed by the user at
any time through the project view.

By default, when opening a project folder, Live CV will automatically try to find the file to run whithin that project
and set it as active, using the following rules:

* Looking for a _main.qml_ file
* If there's no _main.qml_, then the looking for the first _.qml_ file starting with a lower case letter
* If no _.qml_ file starting with a lower case letter is found, then the first found _.qml_ file
* If the project has no _.qml_ files, then the project will be open in edit mode

An important thing to note is that only the **active** file supports all of the live coding features (this
includes runtime editing, palettes and bindings). All the other files have to be **saved** after editing before the
engine will recompile them. This makes sense since the active file is the one that actually instantiates all of the other
components.


## The header

The header contains the following buttons:

* **New** - Creates a new file based project
* **Save As** - Opens a dialog to save the opened file. Note that in order to save the file directly, you can use the
"CTRL+S" key combination.
* **Open File** - Opens a new file. If the file being opened is outside the currently opened project (and the opened
 project is folder based), then the user will be asked whether Live CV should close the current project and scope
to the new one.
* **Open Project Folder** - Opens a new project folder. Live CV will automatically look for the file to run using the
above mentioned rules.
* **Live CV Log Window** - Opens the log window, in which you will either internal messages from Live CV, or custom
debug messages from within the application. The following code for example will show the "Rectangle loaded" message
within the log window.

```
import QtQuick 2.3

Rectangle{
    Component.onCompleted : {
        console.log('Rectangle loaded')
    }
}
```

By default, most error messages will be shown in Live CV's buttom right corner, where a small box with a red border on
the left opens in case of an error. Since some error messages cannot be displayed there, some of them will appear in
the log window, so it's a good ideea to chekc there for any notifications. The button will have a circle on it when a
new log line has been added:

| Log Icon | Log Icon with Message |
| ---------|-----------------------|
|![](images/logicon.jpg) Log Icon | ![](images/logiconmessage.jpg) Log Icon with Message |


* **Show license box** - Opens up the license agreement box. Some algorithms used through Live CV require the user
to accept different licenses, depending on the case. (For example, some may require to be used only in research or
educational purposes). If an algorithm depends on a specific license, the license button will flickr and the user
will be required to accept the license in order to activate the specified component or plugin before continuing.
* **Editor Settings* - Opens the editor settings file for editing. The changes will take effect after the file has
been saved.


## The Project View

The project view displays the currently opened project. You can toggle the view on or off by pressing "CTRL+\".
Within the view you can organize your project by adding or renaming folders and files, editing or
[monitoring files](Monitored_Files) and setting the active or main file to run. These actions are available through the
context menu opened by right clicking on an item within the project. Drag and drop is also supported to move files
and folders around.

Files that are opened are shown in bold. If a file isn't saved, it will have the blue dot on top of it's icon, monitored
files will have the eye symbol next to them and active files will have the green arrow.

![](images/user_running_1.png)

## The Editor

The editor is mostly designed to handle qml and js files. The top of the editor displayes the opened file name, the line and column number and the file navigation button ![](images/user_running_2.png) which opens up the file navigation panel. The file navigation panel initially shows a list of all opened files. When a search is triggered by  typing in the search box, then the file is looked for throughout the project. The files that are opened are always displayed first to provide a faster navigation for the user.

![](images/user_running_3.png)

You can also use the "CTRL+K" key combination to toggle the navigation view.

During code editing, the code completion assist appears automatically. In order to toggle it manually, use the
"CTRL+SPACE" key combination. Items shown for completion are grouped together depending on context, making it easy for
the user to find the item he is looking for. For example, properties of a type are grouped by the types inheritance tree,
each type being displayed on the right side of the menu.

![](images/user_running_4.png)

Object types are grouped by imports as well, where the import name is displyed on the right side of the menu

![](images/user_running_5.png)

The context menu opened by right clicking will show different actions available whithin that context:

![](images/user_running_6.png)

To close the file, use the "x" button next to the file name or press "CTRL+W".

## The Viewer

The viewer is the place where the build application is displayed. It's a simple item that will fill it's adjusted area.
The viewer does not add any scrollbar by default, so applications will have to include them manually in order to provide
scroll functionality.
