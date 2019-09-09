![Live CV](/doc/src/images/logo-dark.png)

[![Join the chat at https://gitter.im/dinusv/livecv](https://badges.gitter.im/dinusv/livecv.svg)](https://gitter.im/dinusv/livecv?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Build Status](https://travis-ci.org/livecv/livecv.svg?branch=master)](https://travis-ci.org/livecv/livecv)
[![Build status](https://ci.appveyor.com/api/projects/status/c1kk7crl0wiox16b?svg=true)](https://ci.appveyor.com/project/dinusv/livecv)

 * **Version**: 1.5.0
 * **License**: LGPL
 * **Website**: [livecv.dinusv.com](http://livecv.dinusv.com)
 * **Demo**: [www.youtube.com/watch?v=uEnJE6Jawfw](https://www.youtube.com/watch?v=uEnJE6Jawfw)
 * **Documentation**: [livecv.dinusv.com/documentation](http://livecv.dinusv.com/documentation)
 * **Build Requirements**:
   * Qt 5.7 or higher
   * Open CV 4.1 or higher


Live CV is a computer vision coding environment that displays results while you code. It can be used to interact, link and adjust algorithms
in order to create quick solutions within it's domain.

The application is built using QtQuick, and uses Open CV library in the back to do the actual processing, kind of like a QML wrapper for the
Open CV library.

An API is also available to link already built algorithms to Live CV's interface by simply exposing configurable variables to Qt's meta object system.

## Installation and getting started

To get started, simply download the application from Live CV's [website](http://livecv.dinusv.com/download.html), extract the archive, and you're ready
to go. Step by step instructions are also found in the download section.

## Compiling

To compile Live CV, you need to have qt and Open CV installed. You can then use qmake to generate the makefile, and run make or nmake to build:

```
cd build
qmake -r
make (or nmake)
```

The executable and plugins will be found in the application directory.

## Contributing

There's an [organization on github](http://github.com/livecv) created for Live CV plugins and projects, so anyone looking to contribute is always welcome. This [section](CONTRIBUTING.md) offers a quick overview on contributing, and there are [simple quickstart issues](https://github.com/dinusv/livecv/issues?q=is%3Aopen+is%3Aissue+label%3Aquickstart) available for anyone looking to start. Most of them deal with wrapping an Open CV functions and adding a sample. There are issues marked with [easy](https://github.com/dinusv/livecv/issues?q=is%3Aopen+is%3Aissue+label%3Aeasy) which are similar, but may have a few more displayable parameters or require a bit more processing.

There's a forum available for discussions on [live cv's website](http://livecv.dinusv.com/forum).

## Changes

For changes and compatibility information between versions, see the [CHANGELOG](CHANGELOG.md).

## Roadmap

Live CV's goal is to provide an environment where people can experiment with different libraries or algorithms and can also share their own as easily as possible. To do this, it needs to be scalable, plugin-based, have a simple and easy to use build system, and a place to access and share plugins. These features ar all planned iteratively in the following versions: 

 * Version 1.6 - Editor Updates
    *  Component documentation from within the editor
    *  Editor shortcut customization
 * Version 1.7 - MultiThreading Module
    * Executing components in separate threads
    * Automatic assignment of components to threads according to their execution time
 * Version 2.0 - Showroom
    * Plugin and sample showroom on Live CV's website
    * Support for fetching plugins and samples from within the application
