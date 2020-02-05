#ifndef QMLDOCUMENTSYNTAXNODE_H
#define QMLDOCUMENTSYNTAXNODE_H

#include <QObject>
#include "live/documentqmlvalueobjects.h"

namespace lv{

class QmlDocumentSyntaxValue : public QObject{

    Q_OBJECT

public:
    explicit QmlDocumentSyntaxValue(
            const DocumentQmlValueObjects::Ptr& vo,
            DocumentQmlValueObjects::RangeItem* item,
            bool objectReference,
            QObject *parent = nullptr);

public slots:
    int begin() const;
    int end() const;
    int typeEnd() const;
    bool isObject() const;
    bool isProperty() const;

private:
    bool                                m_objectReference;
    DocumentQmlValueObjects::Ptr        m_vo;
    DocumentQmlValueObjects::RangeItem* m_item;

};

inline bool QmlDocumentSyntaxValue::isObject() const{
    return m_objectReference;
}

inline bool QmlDocumentSyntaxValue::isProperty() const{
    return !isObject();
}

}// namespace

#endif // QMLDOCUMENTSYNTAXNODE_H
