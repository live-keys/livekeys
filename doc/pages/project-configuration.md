# Project Configuration

A Livekeys package is a collection of Livekeys plugins, usually in the form of a source code repository. A
collection of plugins is grouped together if it serves a similar purpose, or if it serves as a set
of wrappers for the same library. (e.g. lcv* based plugins in Livekeys are all dependent on Open CV library)

In this part we will mostly describe the recommended way to organize such a repository. The
[tutorial repository](https://github.com/live-keys/live-tutorial) can be used as an example.

Previously we have created a plugin with 3 components, under the _tutorial_ uri. The uri
is used when importing the plugin into our application, and also it is taken as the path to our plugin. Livekeys
applies this uri from the _plugins_ folder. So a plugin with the uri `opencv.contrib.core` is expected
to be located in _<livekeys>/plugins/opencv/contrib/core_. This is important, since we want to group our plugins
depending on their context. It's much easier to manage a package if all it's plugins are imported under the same
prefix. Currently Livekeys provides all its plugins without a prefix (e.g. lcvcore instead of opencv.core) for ease
of access, however, it's recommended that packages created for Livekeys should be grouped under the same prefix.

Structuring packages this way, we can group all of our plugins into our plugins folder, for example:

```
plugins/opencv/contrib/core
plugins/opencv/contrib/imgproc
plugins/tutorial
```

Our main repository can then use the plugins folders as subdirs:


```
// livetutorial.pro

TEMPLATE = subdirs
SUBDIRS += plugins
```

And in our plugins, we can further add our plugin:

```
// plugins/plugins.pro

TEMPLATE = subdirs
SUBDIRS += tutorial
```

Since each plugin needs to configure `LIVEKEYS_DEV_PATH` and `LIVEKEYS_BIN_PATH` in our project is a bit of an overhead.
To avoid this, qmake provides a file that is automatically included in all our projects called
_.qmake.conf_. If we add this file to the top of our repository, we can provide functionality to all of our plugins in
one go:

```
// .qmake.conf

LIVEKEYS_BIN_PATH = <path_to_livekeys_installation>
LIVEKEYS_DEV_PATH = $$LIVEKEYS_BIN_PATH/dev

PROJECT_ROOT = $$PWD

include($$LIVEKEYS_DEV_PATH/project/package.pri)
```

The project root is defined as the top level of our repository, and by including the _package.pri_ file, we also add
the _functions.pri_ file we used in the previous parts and also configure our package to be build directly into the
_livekeys/plugins_ directory so we can use it as soon as its built.

The last change we can make is to remove some boiler plate code in our actual plugin file, by including the provided
_plugin.pri_ file in `LIVEKEYS_DEV_PATH/project`:

```
PLUGIN_NAME = tutorial
PLUGIN_PATH = $$PWD

## PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$LIVEKEYS_DEV_PATH/project/plugin.pri)

uri = tutorial
DEFINES += Q_LCV

## Dependencies

linkPlugin(live,    live)
linkPlugin(lcvcore, lcvcore)

# Sources
SOURCES += \
    tutorial_plugin.cpp \
    countnonzeropixels.cpp \
    addweighted.cpp \
    compilationcount.cpp

HEADERS += \
    tutorial_plugin.h \
    countnonzeropixels.h \
    addweighted.h \
    compilationcount.h

DISTFILES = qmldir
```

Using this configuraiton, we are able to set up our package for deployment, and use automated deployment
scripts to create releases.
