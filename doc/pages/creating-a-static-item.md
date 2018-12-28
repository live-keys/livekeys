# Creating a static item

In Live CV, static items are items that keep their internal state across multiple compilations. They are mostly used
in live coding, since during code changes, the application needs to refresh its items and recrete the scene. Items then
have to reset to their initial state, therefore all interactions or changes that occured during the execution of the
application will be lost. The [user section on static items](Static_Items) offers more details about their role and usage.

In this part, we will focus on creating a static item. A simple example would be an item that counts the number of times
the code recompiled during the items lifetime. This would require us to have a single read-only property which we can
use to retrieve this number:

```
Q_PROPERTY(int compilations READ compilations NOTIFY compilationsChanged)
```


To save our state, the live library offers a special container we can use to store our state in, called `QStaticContainer`.
We will retrieve our state from this container when calling the `staticLoad` function. The container groups objects according to their type,
then locates them through their given key. This means that we can have the same key for multiple types, and they will
be stored separately. For example, a VideoDecoder and VideoEncoder state can be stored using the same key, yet if we
were to use the same key for 2 VideoDecoder objects, then one of them will get overwriten. The key is stored as a string,
which we will aquire from the user as part of our `staticLoad` argument. To work with the global `QStaticContainer`
object, we have to link to our live library in _tutorial.pro_ :

```
linkPlugin(live, live)
```
and then we can aquire the static container from our items context:

```
QStaticContainer* container = QStaticContainer::grabFromContext(this);
```
We also need to define the type of state we want our object to store. In this case, we create a custom state type
that stores the number of compilations:

```
class CompilationCountState{
public:
    CompilationCountState():compilations(0){}

    int compilations;
};
```

Then, within the `staticLoad` function, we try to acquire our state, and if no state is present for that given key,
we will create and store it in that location:

```
void staticLoad(const QString &id){

    // grab the QStatiContainer object
    QStaticContainer* container = QStaticContainer::grabFromContext(this);

    // grab our state for the given key
    m_state = container->get<CompilationCountState>(id);
    if ( !m_state ){ // create and store our state if it doesn't exist
        m_state = new CompilationCountState;
        container->set<CompilationCountState>(id, m_state);
    }

    // update the number of compilations
    m_state->compilations++;
    emit compilationsChanged();
}
```

In qml, we now have to call this function when our component is completed:

```
CompilationCount{
    Component.onCompleted : {
        staticLoad('cckey')
        console.log('Compilation count:' + compilations)
    }
}
```

The flow of our application would be as follows:

* CompilationCount is created for the first time. `CompilationCountState` is created and stored at _cckey_.
`compilations` is incremented to 1.
* The code changes and a new scene is about to be created
* CompilationCount object gets destroyed, the state remains stored in the `StaticContainer`
* A new CompilationCount object is created. The object then retrieves the `CompilationCountState` stored at _cckey_
and `compilations` is now incremented to 2.


The full compilationcount class is found below, or you can see the full example in the live-tutorial repository, in
_plugins/tutorial/src/compilationcount.*_.

```
// compilationcount.h

#include <QQuickItem>

class CompilationCountState;
class CompilationCount : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(int compilations READ compilations NOTIFY compilationsChanged)

 public:
    explicit CompilationCount(QQuickItem *parent = 0);

    int compilations() const;

public slots:
    void staticLoad(const QString& id);

signals:
    void compilationsChanged();

private:
    CompilationCountState* m_state;
};
```

```
// compilationcount.cpp

#include "compilationcount.h"
#include "qstaticcontainer.h"

class CompilationCountState{
public:
    CompilationCountState():compilations(0){}

    int compilations;
};

CompilationCount::CompilationCount(QQuickItem *parent)
    : QQuickItem(parent)
    , m_state(0)
{
}

void CompilationCount::staticLoad(const QString &id){
    QStaticContainer* container = QStaticContainer::grabFromContext(this);
    m_state = container->get<CompilationCountState>(id);
    if ( !m_state ){ // create it if it doesn't exist
        m_state = new CompilationCountState;
        container->set<CompilationCountState>(id, m_state);
    }

    m_state->compilations++;

    emit compilationsChanged();
}

int CompilationCount::compilations() const{
    if ( m_state )
        return m_state->compilations;
    return 0;
}
```

Then, we have to register our class to qml:

```
#include "tutorial_plugin.h"
#include "countnonzeropixels.h"
#include "addweighted.h"
#include "compilationcount.h"
#include <qqml.h>

void TutorialPlugin::registerTypes(const char *uri){
    qmlRegisterType<CountNonZeroPixels>(uri, 1, 0, "CountNonZeroPixels");
    qmlRegisterType<AddWeighted>(       uri, 1, 0, "AddWeighted");
    qmlRegisterType<CompilationCount>(  uri, 1, 0, "CompilationCount");
}
```

To test our code, we can add a `Text` type in qml to display our `compilations` property:

```
import QtQuick 2.3
import lcvcore 1.0
import tutorial 1.0

Rectangle{
    Text{
        text : "Compilations : " + cc.compilations
        color : "#fff"
    }
    CompilationCount{
        id : cc
        Component.onCompleted : staticLoad('cc')
    }

}
```
