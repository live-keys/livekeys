#include "hookcontainer.h"
#include "live/visuallogqt.h"

namespace lv{

HookContainer::HookContainer(const QString &projectPath, Runnable* r, QObject *parent)
    : QObject(parent)
    , m_runnable(r)
    , m_projectPath(projectPath)
{
}

HookContainer::~HookContainer(){
}

// Stored:
void HookContainer::insertKey(const QString &file, const QString &id, QObject *obj){
    auto it = m_entries.find(file);
    if ( it == m_entries.end() ){
        it = m_entries.insert(file, QMap<QString, QList<QObject*> >());
    }

    auto idIt = it.value().find(id);
    if ( idIt == it.value().end() ){
        idIt = it.value().insert(id, QList<QObject*>());
    }

    idIt.value().append(obj);
}

}// namespace
