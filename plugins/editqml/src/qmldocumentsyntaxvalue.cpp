#include "qmldocumentsyntaxvalue.h"

namespace lv{

QmlDocumentSyntaxValue::QmlDocumentSyntaxValue(
        const DocumentQmlValueObjects::Ptr &vo,
        DocumentQmlValueObjects::RangeItem *item,
        bool objectReference,
        QObject *parent)
    : QObject(parent)
    , m_objectReference(objectReference)
    , m_vo(vo)
    , m_item(item)
{
}


int QmlDocumentSyntaxValue::begin() const{
    if ( m_objectReference ){
        auto ob = static_cast<DocumentQmlValueObjects::RangeObject*>(m_item);
        return ob->begin;
    } else {
        return 0;
    }
}

int QmlDocumentSyntaxValue::end() const{
    if ( m_objectReference ){
        auto ob = static_cast<DocumentQmlValueObjects::RangeObject*>(m_item);
        return ob->end;
    } else {
        return 0;
    }
}

int QmlDocumentSyntaxValue::typeEnd() const{
    if ( m_objectReference ){
        auto ob = static_cast<DocumentQmlValueObjects::RangeObject*>(m_item);
        return ob->identifierEnd;
    } else {
        return 0;
    }
}

}// namespace
