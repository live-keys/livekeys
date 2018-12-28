
# Creating a Plugin

These 4 sections will take you through the basics on building a Live CV plugin. A Live CV plugin is a QtQuick based
plugin with the added difference that it may have external dependencies outside of Qt (Open CV, dlib, etc)
This makes Qt based plugins compatible in Live CV, however to use Live CV plugins in a QtQuick application
the user must make sure that the application can find the plugins dependencies (which is done automatically by
Live CV). Before starting, it's important to know the structure and contents of
[plugins and plugin packages](Plugins), which is detailed in the user section.

The full project we will be building is also available at [this github repository](https://github.com/livecv/live-tutorial), so if you want to just
start hacking through an existing project or get stuck on something, you can use the repository as a reference. The
repository is also configured as a fully deployable package, which will be described in detail within the [project configuration](Project_Configuration) section.

To start off, we can use the available template project in QtCreator, which will create our plugin
template and our first item. So, in QtCreator, begin by creating a new project. Choose **library**, then
[Qt Quick 2 Extension Plugin]().

![](../src/images/api_creating_1.png)

Set up the location, and put in the name "tutorial" as the project name.

![](../src/images/api_creating_2.png)

Next, select the compiler. The third step will ask you an object class-name, and an URI. The first item we will be
building is an item that will count the number of pixels whithin a matrix. We will call it CountNonZeroPixels. So
set up the object class-name to be "CountNonZeroPixels", and **make sure to set the URI** to "tutorial"
(or <module_name> in case you named the module diferently).

![](../src/images/api_creating_3.png)

This path will be required by Live CV when it loads the plugin. From here go to the last step and click **Finish**. You
will notice a project with 2 c++ classes set up. The first one is the tutorial_plugin class, which contains a hook
method to register all the plugin types or QML items once the plugin loads, and the actual QtQuick item that will do
the pixel counting.

If we build this plugin, although it doesn't provide any functionality, we can already use it in Live CV. We can test
this if we build the project, and from our build directory we copy the library files ( *.so on linux or *.dll on
windows) together with your qmldir file into your livecv installation directory, in plugins/tutorial. So we should
have the following file structure:
* livecv/plugins/tutorial/tutorial.so
* livecv/plugins/tutorial/qmldir

or on Windows:
* livecv/plugins/tutorial/tutorial.dll
* livecv/plugins/tutorial/qmldir

Next, if we open Live CV and import the tutorial plugin, we can create a `CountNonZeroPixels` object:

```
import QtQuick 2.3
import tutorial 1.0

Grid{
    CountNonZeroPixels{}
}
```

Although this object doesn't do anything for now, the fact that we are able to create it without any errors
means that we are able to use our plugin successfully.

If you want to build QtQuick based plugins without any dependencies, from here you can simply start adding your items
and configure their properties.

In the [following part](Creating_a_Live_CV_based_Plugin) we will look into linking our plugin to Open CV library and
start working with Live CV types to do the actual pixel counting.
