# Editor

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