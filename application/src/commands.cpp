#include "commands.h"
#include <QJSValueIterator>
#include <QDebug>

namespace lv{

Commands::Commands(QObject *parent)
    : QObject(parent)
    , m_root(new Node)
{
}

Commands::~Commands(){
    delete m_root;
}

QString Commands::dump(){
    return m_root->recurseDump();
}

QString Commands::add(QObject *object, const QJSValue &commands){
    if ( object == 0 || object->objectName() == "" ){
        qCritical("Cannot add commands for unnamed objects.");
        return "";
    }
    if ( !commands.isObject() ){
        qCritical("Commands requires to be of object type.");
        return "";
    }

    QObjectList commandChain = getCommandChain(object);
    Commands::Node* current = m_root;
    for ( auto it = commandChain.begin(); it != commandChain.end(); ++it ){
        current = current->add(*it);
        if ( current == 0 )
            return "";
    }

    QJSValueIterator vit(commands);
    while( vit.hasNext() ){
        vit.next();
        if ( vit.value().isCallable() ){
            current->functions[vit.name()] = vit.value();
        } else {
            qCritical("Non function value added: %s", qPrintable(vit.value().toString()));
        }
    }

    QString path = "";
    for( auto it = commandChain.begin(); it != commandChain.end(); ++it ){
        path += (path.isEmpty() ? "" : ".") + (*it)->objectName();
    }

    connect(object, SIGNAL(destroyed(QObject*)), this, SLOT(removeCommandsFor(QObject*)));

    return path;
}

void Commands::removeCommandsFor(QObject *object){
    QObjectList commandChain = getCommandChain(object);
    if ( commandChain.isEmpty() )
        return;

    Commands::Node* currentNode = m_root;
    for ( auto it = commandChain.begin(); it != commandChain.end(); ++it ){
        QObject* itob = *it;
        if ( itob == object ){
            currentNode->remove(itob);
            return;
        }
        currentNode = currentNode->find(itob);
        if ( !currentNode )
            return;
    }
}

void Commands::execute(const QString &command){
    QStringList commandChain = command.split(".");
    if ( commandChain.isEmpty() )
        return;
    QStringList objectChain = commandChain;
    objectChain.removeLast();

    // find commands and their object.
    QList<QPair<Commands::Node*, bool> > base = m_root->recurseFind(objectChain, objectChain.begin());

    // if it's a single command, execute it
    if ( base.size() == 1 ){
        base.first().first->functions[commandChain.last()].call();

    // if there are more than two commands, execute the one in focus
    } else if ( base.size() > 1 ){
        for ( auto it = base.begin(); it != base.end(); ++it ){
            if ( it->second == true ){
                it->first->functions[commandChain.last()].call();
                return;
            }
        }
    }
}

QObjectList Commands::getCommandChain(QObject *object){
    QObjectList commandChain;
    while( object != 0 ){
        if ( object->objectName() != "" )
            commandChain.prepend(object);
        object = object->parent();
    }

    return commandChain;
}

Commands::Node::~Node(){
    for ( auto it = nodes.begin(); it != nodes.end(); ++it )
        delete it.value();
}

Commands::Node *Commands::Node::find(QObject *object){
    if ( object == 0 || object->objectName().isEmpty() )
        return 0;

    auto nodeit = nodes.find(object->objectName());

    while (nodeit != nodes.end() && nodeit.key() == object->objectName()){
        if ( nodeit.value()->object == object )
            return nodeit.value();
        ++nodeit;
    }

    return 0;
}

Commands::Node *Commands::Node::add(QObject *object){
    if ( object->objectName().isEmpty() )
        return 0;

    Commands::Node* obj = find(object);
    if ( obj )
        return obj;

    Commands::Node* node = new Commands::Node;
    node->object = object;
    nodes.insert(object->objectName(), node);

    return node;
}

void Commands::Node::remove(QObject *object){
    if ( object == 0 || object->objectName().isEmpty() )
        return;

    auto nodeit = nodes.find(object->objectName());

    while (nodeit != nodes.end() && nodeit.key() == object->objectName()){
        if ( nodeit.value()->object == object ){
            nodes.erase(nodeit);
            return;
        }
    }
}

QList<QPair<Commands::Node *, bool> > Commands::Node::recurseFind(
        const QStringList &list,
        QStringList::const_iterator it,
        bool hasFocus)
{
    QList<QPair<Commands::Node *, bool> > base;

    if ( object != 0 ){
        QVariant focus = object->property("focus");
        if ( focus.type() == QVariant::Bool && focus.value<bool>() == true ){
            hasFocus = true;
        }
    }

    auto nodeit = nodes.find(*it);

    QStringList::const_iterator nextit = it;
    ++nextit;

    while (nodeit != nodes.end() && nodeit.key() == *it) {
        if ( nextit == list.end() ){
            base.append(QPair<Commands::Node*, bool>(nodeit.value(), hasFocus));
        } else {
            base.append(nodeit.value()->recurseFind(list, nextit, hasFocus));
        }
        ++nodeit;
    }

    return base;
}

QString Commands::Node::recurseDump(QString prefix){
    QString base;
    if ( object )
        base += prefix + ":" + QString::number(functions.size()) + "\n";

    for ( auto it = nodes.begin(); it != nodes.end(); ++it ){
        base += it.value()->recurseDump(prefix + (prefix.isEmpty() ? "" : ".") + it.key());
    }

    return base;
}

}// namespace
