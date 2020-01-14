#include "qmlcomponentmap.h"
#include "qmlcomponentmapdata.h"
#include <QQmlEngine>
#include <QQmlContext>

namespace lv{

namespace{

template<class T>
void resizeList(QList<T> & list, int newSize) {
    int diff = newSize - list.size();
    T t;
    if (diff > 0) {
        list.reserve(diff);
        while (diff--) list.append(t);
    } else if (diff < 0) list.erase(list.end() + diff, list.end());
}

}

QmlComponentMap::QmlComponentMap(QObject *parent)
    : QObject(parent)
    , m_isProcessing(false)
    , m_f(nullptr)
{
}

QmlComponentMap::~QmlComponentMap(){
    clearCurrent();
}

void QmlComponentMap::process(){
    if ( !m_input.size() || !m_f || m_isProcessing){
        return;
    }

    m_isProcessing = true;

    clearCurrent();
    m_output = QVariantList();

    resizeList(m_output, m_input.size());

    for (auto it = m_input.begin(); it != m_input.end(); ++it ){
        QmlComponentMapData* dt = new QmlComponentMapData(this);
        m_fData.append(dt);

        QQmlContext* ctx = new QQmlContext(m_f->creationContext(), this);
        ctx->setContextProperty("map", dt);

        m_fContexts.append(ctx);

        QObject* obj = qobject_cast<QObject*>(m_f->create(ctx));
        if ( obj == 0 ){
            qCritical("MapArray: Failed to create item: %s", qPrintable(m_f->errorString()));
            return;
        }

        m_fObjects.append(obj);
    }
}

void QmlComponentMap::assignResult(QmlComponentMapData *mad, const QVariant &v){
    m_output[mad->index()] = v;
    mad->setResult(true);

    // all results should be true
    for ( auto it = m_fData.begin(); it != m_fData.end(); ++it ){
        if ( !(*it)->result() )
            return;
    }

    m_isProcessing = false;

    emit outputChanged();
}

void QmlComponentMap::clearCurrent(){
    for ( auto it = m_fObjects.begin(); it != m_fObjects.end(); ++it )
        (*it)->deleteLater();
    for ( auto it = m_fContexts.begin(); it != m_fContexts.end(); ++it )
        (*it)->deleteLater();
    for ( auto it = m_fData.begin(); it != m_fData.end(); ++it )
        (*it)->deleteLater();
    m_fObjects.clear();
    m_fContexts.clear();
    m_fData.clear();
}


} // namespace
