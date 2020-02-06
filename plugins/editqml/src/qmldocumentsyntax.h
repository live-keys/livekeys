#ifndef QMLDOCUMENTSYNTAX_H
#define QMLDOCUMENTSYNTAX_H

#include <QObject>

#include "live/projectdocument.h"
#include "live/documentqmlinfo.h"
#include "live/documentqmlvalueobjects.h"
#include "live/projectfile.h"

#include "qmldocumentsyntaxvalue.h"

namespace lv{

class QmlDocumentSyntax : public QObject{

    Q_OBJECT
    Q_PROPERTY(ProjectDocument* document READ document WRITE setDocument NOTIFY documentChanged)

public:
    explicit QmlDocumentSyntax(QObject *parent = nullptr);
    ~QmlDocumentSyntax();

    ProjectDocument* document() const;
    void setDocument(ProjectDocument* document);

signals:
    void documentChanged();

public slots:
    lv::QmlDocumentSyntaxValue* findId(const QString& id);
    lv::QmlDocumentSyntaxValue* objectAroundPosition(int position);

private:
    QmlDocumentSyntaxValue* findId(const QString& id, DocumentQmlValueObjects::RangeObject* ro);
    DocumentQmlValueObjects::RangeObject *objectAroundPosition(int position, DocumentQmlValueObjects::RangeObject* ro);

    void syncDocument();

    bool                         m_documentInfoDirty;
    ProjectDocument*             m_document;
    DocumentQmlInfo::Ptr         m_documentInfo;
    DocumentQmlValueObjects::Ptr m_documentObjects;
};

inline ProjectDocument *QmlDocumentSyntax::document() const{
    return m_document;
}

inline void QmlDocumentSyntax::setDocument(ProjectDocument *document){
    if (m_document == document)
        return;

    m_documentInfoDirty = true;
    m_document = document;
    m_documentObjects = nullptr;
    m_documentInfo = DocumentQmlInfo::create(document->file()->path());
    emit documentChanged();
}

}// namespace

#endif // QMLDOCUMENTSYNTAX_H
