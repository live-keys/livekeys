# Contributing to Live CV

This guide will serve as a quick start for anyone interested in contributing with Live CV. It offers a quick view over:

1. The Structure of Live CV, 
2. Qml Components from c++
3. Repository organization

## Structure

Live CV is split into 3 sections:

 * the application itself containing the editor and viewer
 * lcvlib library, which all Live CV plugins link to. It contains the main components that provide a link between Open CV and qml
 * the plugins or modules, containing computer vision plugins that can be imported in Live CV (lcvcore, lcvimgproc, ...)
 
## QML components from c++

This section is just quick overview on how to create a qml component from c++. For a more detailed description on qml language and its bindings, you can check out the following links:

 * [Tutorial on qml](http://doc.qt.io/qt-5/qml-tutorial1.html)
 * [QML Types from C++](http://doc.qt.io/qt-5/qtqml-cppintegration-definetypes.html)
 * [Writing QML Extensions](http://doc.qt.io/qt-5/qtqml-tutorials-extending-qml-example.html)
 * [Creating a Live CV filter](http://livecv.dinusv.com/documentation/api-creating-a-filter.html)

In terms of c++, qml components are basically classes of a [QObject](http://doc.qt.io/qt-5/qobject.html) subtype registered to the qml meta object system. A component is registered using the [qmlRegisterType](http://doc.qt.io/qt-5/qqmlengine.html#qmlRegisterType-2) function:

```cpp
int qmlRegisterType(const QUrl &url, const char *uri, int versionMajor, int versionMinor, const char *qmlName)
```

For example, in lcvimgproc plugin, at [plugins/lcvimgproc/src/lcvimgproc_plugin.cpp](https://github.com/dinusv/livecv/blob/master/plugins/lcvimgproc/src/lcvimgproc_plugin.cpp) you can see how each component is registered:

```cpp
void LcvimgprocPlugin::registerTypes(const char *uri){ 
  // @uri modules.lcvimgproc 
  qmlRegisterType<QBlur>(uri, 1, 0, "Blur"); 
  qmlRegisterType<QCanny>(uri, 1, 0, "Canny"); 
  qmlRegisterType<QGaussianBlur>(uri, 1, 0, "GaussianBlur"); 
  qmlRegisterType<QHoughLines>(uri, 1, 0, "HoughLines"); 
  // ...
} 
```

Whenever the plugin gets imported, the function gets called and the components become available. Components can have a set of properties accessible through qml. A property is declared through the Q_PROPERTY macro:

```
Q_PROPERTY(Type value READ getValue WRITE setValue NOTIFY valueChanged)
```

The READ, WRITE and NOTIFY declare the getter setter and property changed handler. So, to declare a simple qml component that has a ```int``` property defaulted to 42:

```
#include <QObject>

class MySample : public QObject{

  Q_OBJECT
  Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
  
public:
  MySample(QObject* parent) : QObject(parent), m_value(42){}
  
  int value() const {return m_value;}
  
  void setValue(int value){
    if ( m_value != value ){
      m_value = value;
      emit valueChanged();
    }
  }
  
signals:
  void valueChanged();
  
private:
  int m_value;
};
```

The component can then be used in qml:

```qml
MySample{
  Component.onCompleted : {
    console.log(value);
  }
}
```

## Repository

Live CV's repository contains the master and development branch. The master branch is considered the stable branch, with all integrated components tested and functional. The **dev** branch is used for the development of Live CV. The branch contains the latest added components, which are not yet fully stable.

The coding style used for Live CV si compatible with Qt's standards, described [here](https://wiki.qt.io/Coding_Conventions), with the addition of exceptions in some cases, for example coming from Open CV.

Issues have associated labels, some of them handling specific cases:

 * deployment : dealing with Live CV project files, build and deployment scripts
 * quickstart : used for people who want to start contributing to Live CV
 * easy : entry level or issues that don't require a lot of time
 * editor : related to Live CV's code editor
 * has-sample : requires a use case sample within Live CV's sample directory
 * opencv : is related to open cv or an open cv wrapper
 
If issues are part of a module, the module is usually speciifed at the begining of the issue, e.g. [lcvvideo] Add BackgroundSubtractor{}
