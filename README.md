<p style="text-align: center;">
    <a href="https://livekeys.io/">
        <img
          alt="Livekeys"
          src="https://github.com/live-keys/livekeys/blob/master/doc/src/images/logo.png"
          width="400"
        />
   </a>
</p>

[![Join the chat at https://gitter.im/live-keys/livekeys](https://badges.gitter.im/live-keys/livekeys.svg)](https://gitter.im/live-keys/livekeys?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

 * **Version**: 1.6.0
 * **License**: LGPL
 * **Website**: [livekeys.io](https://livekeys.io)
 * **Demo**: [www.youtube.com/watch?v=ZPt5KSy1wh0](https://www.youtube.com/watch?v=ZPt5KSy1wh0)
 * **Documentation**: [livekeys.io/docs](https://livekeys.io/docs)
 * **Build Requirements**:
   * Qt 5.7 or higher
   * Open CV 4.1 or higher


Livekeys is a live coding environment based on components. The application uses Qml as the main language, and can be used to create quick solutions within a wide variety of domains. Main features include:

 * **Live coding**: Everything you type is compiled automatically

 * **Palettes and bindings**: Palettes are visual items that provide a connection between the code and runtime. They
   control values they bind to and also, in reverse, monitor the value within the application. They are customizable
   per type.

 * **Component based**: Components, which can handle a variety of roles, are declarative and communicate with each other through connections, making them swappable depending on the context.
   
 * **Extendable**: Components are wrapped in plugins, then packages, which are installable through a package manager.

Although the main focus has been on integrating OpenCV for computer vision tasks, other libraries can be integrated
easily using Livekeys's API.

The project is the successor of [Live CV](https://www.youtube.com/watch?v=2zTY6CFhP_A).


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

 * Version 1.7 - Editor Updates
    * Component documentation from within the editor
 * Version 1.8 - MultiThreading Module
    * Executing components in separate threads
    * Automatic assignment of components to threads according to their execution time
 * Version 2.0 - Package Manager Details
    * Plugin and sample showroom on Live Keys's website
    * Support for fetching plugins and samples from within the application
