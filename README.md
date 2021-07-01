<p align="center" style="text-align: center;">
    <a href="https://livekeys.io/">
        <img
          alt="Livekeys"
          src="https://github.com/live-keys/livekeys/blob/master/doc/src/images/logo.png"
          width="400"
        />
   </a>
</p>

[![Join the chat at https://gitter.im/live-keys/livekeys](https://badges.gitter.im/live-keys/livekeys.svg)](https://gitter.im/live-keys/livekeys?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Build Status](https://travis-ci.com/live-keys/livekeys.svg?branch=master)](https://travis-ci.com/live-keys/livekeys)
[![Build status](https://ci.appveyor.com/api/projects/status/3l5t69h7q3gpkec1?svg=true)](https://ci.appveyor.com/project/dinusv/livekeys)

 * **Version**: 1.9.1
 * **License**: LGPL
 * **Website**: [livekeys.io](https://livekeys.io)
 * **Demo**: [www.youtube.com/watch?v=ZPt5KSy1wh0](https://www.youtube.com/watch?v=ZPt5KSy1wh0)
 * **Documentation**: [livekeys.io/docs](https://livekeys.io/docs)
 * **Build Requirements**:
   * Qt 5.12 or higher
   * Open CV 4.1 or higher

Livekeys is a platform that automates content creation in real time by combining visual scripting with live coding.
Components are used to create pipelines for familiarized tasks (file reading, video editing, computer vision modules, etc), providing configuration via
their user interfaces. To extend this pipeline, users can build their own components via live coding.
Main features include:

 * **Image and Video Processing**: Load image/video files and applying various filters

 * **Node Editor** : Connect and configure components via nodes in a graph

 * **Timeline Editor**: Schedule animations and organize video segments

 * **Code editor**: Syntax highlighting, code completion, live coding, and seamless switching between UI configuration and code

 * **Interactive learning module**: Provides tutorials with interactions that guide you through the process of learning Livekeys.
   
 * **Plugin support**: Components are wrapped in plugins, then packages, which are installable through a package manager.


## Installation and getting started

To get started, simply download the application from Livekeys's [website](https://livekeys.io/download), extract the
archive, and you're ready to go. Step by step instructions are also found in the download section.

## Compiling

To compile Livekeys, you need to have Qt and Open CV installed. You can then use `qmake` to generate the makefile,
and run make or `nmake` to build:

```
cd <livekeys_path>/build
qmake -r ..
make (or nmake)
```

The executable and plugins will be found in the bin directory.

## Contributing

There's an [organization on github](http://github.com/live-keys) created for Livekeys plugins and projects, so anyone
looking to contribute is always welcome. This [section](CONTRIBUTING.md) offers a quick overview on contributing,
and there are [simple quickstart issues](https://github.com/live-keys/livekeys/issues?q=is%3Aopen+is%3Aissue+label%3Aquickstart)
available for anyone looking to start. Most of them deal with wrapping an Open CV functions and adding a sample.
There are issues marked with [easy](https://github.com/live-keys/livekeys/issues?q=is%3Aopen+is%3Aissue+label%3Aeasy) which are
similar, but may have a few more displayable parameters or require a bit more processing.

There's a chat room available on [gitter](https://gitter.im/live-keys/livekeys) and also a [forum](https://livekeys.io/forum).

## Changes

For changes and compatibility information between versions, see the [CHANGELOG](CHANGELOG.md).

## Roadmap

Livekeys's goal is to provide an environment where people can experiment with different libraries or algorithms and
can also share their own as easily as possible. To do this, it needs to be scalable, plugin-based, have a simple and
easy to use build system, and a place to access and share plugins. Our goal is to reach this in version 2.0 of the
application.

 * Version 1.10 - MultiThreading Module
    * Async execution
 * Version 2.0 - Package Manager Details
    * Plugin and sample showroom on Live Keys's website
    * Support for fetching plugins and samples from within the application
