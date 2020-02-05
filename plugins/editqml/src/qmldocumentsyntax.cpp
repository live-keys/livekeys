#include "qmldocumentsyntax.h"
#include "live/visuallogqt.h"

namespace lv{

QmlDocumentSyntax::QmlDocumentSyntax(QObject *parent)
    : QObject(parent)
    , m_documentInfoDirty(false)
    , m_document(nullptr)
{
}

QmlDocumentSyntax::~QmlDocumentSyntax(){
}

QmlDocumentSyntaxValue *QmlDocumentSyntax::findId(const QString &id){
    syncDocument();
    if ( !m_documentObjects )
        return nullptr;

    DocumentQmlValueObjects::RangeObject* root = m_documentObjects->root();
    return findId(id, root);
}

QmlDocumentSyntaxValue *QmlDocumentSyntax::objectAroundPosition(int position){
    syncDocument();
    if ( !m_documentObjects )
        return nullptr;

    DocumentQmlValueObjects::RangeObject* root = m_documentObjects->root();
    DocumentQmlValueObjects::RangeObject* result = objectAroundPosition(position, root);
    if ( result )
        return new QmlDocumentSyntaxValue(m_documentObjects, result, true);
    return nullptr;
}

QmlDocumentSyntaxValue *QmlDocumentSyntax::findId(const QString &id, DocumentQmlValueObjects::RangeObject *ob){
    for ( int i = 0; i < ob->properties.size(); ++i ){
        if ( ob->properties[i]->name().length() == 1 && ob->properties[i]->name().first() == "id" ){
            int f = ob->properties[i]->valueBegin;
            int l = ob->properties[i]->end;
            QString propValue = m_document->substring(f, l - f);
            if ( propValue == id )
                return new QmlDocumentSyntaxValue(m_documentObjects, ob, true);
        }
    }

    for ( int i = 0; i < ob->children.size(); ++i ){
        QmlDocumentSyntaxValue* r = findId(id, ob->children[i]);
        if ( r )
            return r;
    }
    return nullptr;
}

DocumentQmlValueObjects::RangeObject *QmlDocumentSyntax::objectAroundPosition(int position, DocumentQmlValueObjects::RangeObject *ob){
    if ( ob->identifierEnd <= position && ob->end > position ){
        for ( int i = 0; i < ob->children.size(); ++i ){
            DocumentQmlValueObjects::RangeObject * r = objectAroundPosition(position, ob->children[i]);
            if ( r )
                return r;
        }
        return ob;
    }
    return nullptr;
}

void QmlDocumentSyntax::syncDocument(){
    if ( m_documentInfoDirty && m_document ){
        m_documentInfo->parse(m_document->content());
        m_documentObjects   = m_documentInfo->createObjects();
        m_documentInfoDirty = false;
    }
}

}// namespace
