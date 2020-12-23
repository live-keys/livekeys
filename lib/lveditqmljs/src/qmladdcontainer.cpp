#include "qmladdcontainer.h"

namespace lv{

QmlAddContainer::QmlAddContainer(int addPosition, const QmlTypeReference &objectType, QObject *parent)
    : QObject(parent)
    , m_objectType(objectType)
    , m_model(new QmlSuggestionModel(addPosition, this))
{
}

QmlAddContainer::~QmlAddContainer(){
    delete m_model;
}

} // namespace
