#include "qmladdcontainer.h"

namespace lv{

QmlAddContainer::QmlAddContainer(int addPosition, const QStringList &objectType, QObject *parent)
    : QObject(parent)
    , m_objectTypePath(objectType)
    , m_model(new QmlSuggestionModel(addPosition, this))
{
}

QmlAddContainer::~QmlAddContainer(){
    delete m_model;
}

} // namespace
