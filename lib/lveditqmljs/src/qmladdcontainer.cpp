#include "qmladdcontainer.h"

namespace lv{

QmlAddContainer::QmlAddContainer(int addPosition, const QStringList &objectType, QObject *parent)
    : QObject(parent)
    , m_objectTypePath(objectType)
    , m_itemModel(new QmlSuggestionModel(addPosition, this))
    , m_propertyModel(new QmlSuggestionModel(addPosition, this))
    , m_eventModel(new QmlSuggestionModel(addPosition, this))
{
}

QmlAddContainer::~QmlAddContainer(){
    delete m_propertyModel;
    delete m_itemModel;
    delete m_eventModel;
}

} // namespace
