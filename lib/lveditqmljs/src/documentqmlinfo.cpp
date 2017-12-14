/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#include "live/documentqmlinfo.h"
#include "live/documentqmlvalueobjects.h"
#include "live/codedeclaration.h"
#include "live/coderuntimebinding.h"
#include "live/projectdocument.h"
#include "live/projectfile.h"

#include "qmljs/qmljsdocument.h"
#include "qmljs/qmljsbind.h"
#include "qmlidvisitor_p.h"
#include "documentqmlranges_p.h"

#include <QQmlProperty>
#include <QQmlListReference>

namespace lv{

namespace{

    QQmlProperty findRuntimeProperty(
        DocumentQmlValueObjects::RangeObject *object,
        QObject *root,
        CodeDeclaration::Ptr declaration,
        const QString& source)
    {

        int position = declaration->position();

        for ( int i = 0; i < object->properties.size(); ++i ){
            if ( object->properties[i]->begin == position ){

                if ( declaration->identifierChain().isEmpty() ){
                    return QQmlProperty();
                }

                // iterate property chain (eg. border.size => (border, size))
                QObject* objectChain = root;
                for ( int j = 0; j < declaration->identifierChain().size() - 1; ++j ){
                    QQmlProperty foundprop(root, declaration->identifierChain()[j]);
                    if ( !foundprop.isValid() || foundprop.propertyTypeCategory() != QQmlProperty::Object ){
                        return QQmlProperty();
                    }

                    objectChain = foundprop.read().value<QObject*>();
                }

                // found property
                declaration->setValuePositionOffset(
                    object->properties[i]->valueBegin -
                    object->properties[i]->begin -
                    declaration->identifierLength()
                );

                declaration->setValueLength(
                    object->properties[i]->end - object->properties[i]->valueBegin
                );

                return QQmlProperty(objectChain, declaration->identifierChain().last());

            } else if ( object->properties[i]->child &&
                        object->properties[i]->begin < position &&
                        object->properties[i]->end > position )
            {
                return findRuntimeProperty(
                    object->properties[i]->child, root, declaration, source
                );
            }
        }

        QQmlProperty pp(root);
        if ( pp.isValid() ){
            if ( pp.propertyTypeCategory() == QQmlProperty::Object &&
                 object->children.size() == 1 &&
                 position > object->children[0]->begin &&
                 position < object->children[0]->end )
            {
                return findRuntimeProperty(
                    object->children[0], pp.read().value<QObject*>(), declaration, source
                );
            } else if ( pp.propertyTypeCategory() == QQmlProperty::List ){
                QQmlListReference ppref = qvariant_cast<QQmlListReference>(pp.read());

                // check if have children and object hierarchy hasn't been modified
                if ( ppref.canAt() && ppref.canCount() && ppref.count() == object->children.size() ){
                    for ( int i = 0; i < object->children.size(); ++i ){
                        if ( position > object->children[i]->begin &&
                             position < object->children[i]->end &&
                             ppref.count() > i )
                        {
                            return findRuntimeProperty( object->children[i], ppref.at(i), declaration, source );
                        }
                    }
                }
            }
        }

        return QQmlProperty();
    }
}


class DocumentQmlInfoPrivate{
public:
    QmlJS::Document::MutablePtr      internalDoc;
    QmlJS::Bind*                     internalDocBind;
    DocumentQmlRanges               ranges;
    QList<DocumentQmlInfo::Message> messages;
};

DocumentQmlInfo::Dialect DocumentQmlInfo::extensionToDialect(const QString &extension){
    static QHash<QString, DocumentQmlInfo::Dialect> map;
    map["js"]         = DocumentQmlInfo::Javascript;
    map["qml"]        = DocumentQmlInfo::Qml;
    map["qmltypes"]   = DocumentQmlInfo::QmlTypeInfo;
    map["qmlproject"] = DocumentQmlInfo::QmlProject;
    map["json"]       = DocumentQmlInfo::Json;

    return map.value(extension, DocumentQmlInfo::Unknown);
}

DocumentQmlInfo::DocumentQmlInfo(const QString &fileName)
    : d_ptr(new DocumentQmlInfoPrivate)
{
    Q_D(DocumentQmlInfo);
    DocumentQmlInfo::Dialect dialect = extensionToDialect(QFileInfo(fileName).suffix());
    if ( dialect == DocumentQmlInfo::Javascript )
        d->internalDoc = QmlJS::Document::create(fileName, QmlJS::Dialect::JavaScript);
    else if ( dialect == DocumentQmlInfo::Qml || fileName == "" ){
        d->internalDoc = QmlJS::Document::create(fileName, QmlJS::Dialect::Qml);
    } else
        d->internalDoc = QmlJS::Document::create(fileName, QmlJS::Dialect::NoLanguage);
}

DocumentQmlInfo::Ptr DocumentQmlInfo::create(const QString &fileName){
    return DocumentQmlInfo::Ptr(new DocumentQmlInfo(fileName));
}

QStringList DocumentQmlInfo::extractIds() const{
    Q_D(const DocumentQmlInfo);
    if ( d->internalDocBind->idEnvironment() == 0 )
        return QStringList();

    IdExtractor extractor;
    d->internalDocBind->idEnvironment()->processMembers(&extractor);
    return extractor.ids();
}

const DocumentQmlInfo::ValueReference DocumentQmlInfo::rootObject(){
    Q_D(DocumentQmlInfo);
    return DocumentQmlInfo::ValueReference(d->internalDocBind->rootObjectValue(), this);
}

const DocumentQmlInfo::ValueReference DocumentQmlInfo::valueForId(const QString &id) const{
    Q_D(const DocumentQmlInfo);
    if ( d->internalDocBind->idEnvironment() == 0 )
        return DocumentQmlInfo::ValueReference();

    IdValueExtractor valueExtractor(id);
    d->internalDocBind->idEnvironment()->processMembers(&valueExtractor);
    return DocumentQmlInfo::ValueReference(valueExtractor.value(), this);
}

DocumentQmlObject DocumentQmlInfo::extractValueObject(
        const ValueReference &valueref,
        ValueReference *parent) const
{
    DocumentQmlObject vodata;
    if ( isValueNull(valueref) || valueref.parent != this )
        return vodata;

    if ( const QmlJS::ASTObjectValue* vob = valueref.value->asAstObjectValue() ){
        if ( vob->typeName() )
            vodata.setTypeName(vob->typeName()->name.toString());

        ValueMemberExtractor extractor(&vodata);
        vob->processMembers(&extractor);
        if ( parent ){
            parent->value = extractor.parent();
            parent->parent = this;
        }
    }

    return vodata;
 }

QString DocumentQmlInfo::extractTypeName(const DocumentQmlInfo::ValueReference &valueref) const{
    if ( isValueNull(valueref) || valueref.parent != this )
        return "";

    if ( const QmlJS::ASTObjectValue* vob = valueref.value->asAstObjectValue() )
        if ( vob->typeName() )
            return vob->typeName()->name.toString();

    return "";
}

void DocumentQmlInfo::createRanges(){
    Q_D(DocumentQmlInfo);
    d->ranges(d->internalDoc);
}

const DocumentQmlInfo::ValueReference DocumentQmlInfo::valueAtPosition(int position) const{
    Q_D(const DocumentQmlInfo);

    DocumentQmlRanges::Range range = d->ranges.findClosestRange(position);
    if ( range.ast == 0 )
        return DocumentQmlInfo::ValueReference();

    QmlJS::ObjectValue* value = d->internalDocBind->findQmlObject(range.ast);
    return DocumentQmlInfo::ValueReference(value, this);
}

const DocumentQmlInfo::ValueReference DocumentQmlInfo::valueAtPosition(
        int position,
        int &begin,
        int &end) const
{
    Q_D(const DocumentQmlInfo);

    QList<DocumentQmlRanges::Range> rangePath = d->ranges.findRangePath(position);
    foreach( const DocumentQmlRanges::Range& range, rangePath ){
        begin = range.begin;
        end   = range.end;

//        QmlJS::ObjectValue* value = d->internalDocBind->findQmlObject(range.ast);
//        return QDocumentQmlInfo::ValueReference(value, this);
    }

    DocumentQmlRanges::Range range = d->ranges.findClosestRange(position);
    if ( range.ast == 0 )
        return DocumentQmlInfo::ValueReference();

    begin = range.begin;
    end   = range.end;

    QmlJS::ObjectValue* value = d->internalDocBind->findQmlObject(range.ast);
    return DocumentQmlInfo::ValueReference(value, this);
}

bool DocumentQmlInfo::isValueNull(const DocumentQmlInfo::ValueReference& vr) const{
    return vr.value == 0;
}

bool DocumentQmlInfo::isParsedCorrectly() const{
    Q_D(const DocumentQmlInfo);
    return d->internalDoc->isParsedCorrectly();
}

bool DocumentQmlInfo::parse(const QString &source){
    Q_D(DocumentQmlInfo);
    d->messages.clear();
    d->internalDoc->setSource(source);
    bool parseResult = d->internalDoc->parse();
    d->internalDocBind = d->internalDoc->bind();

    foreach( const QmlJS::DiagnosticMessage& message, d->internalDoc->diagnosticMessages() ){
        DocumentQmlInfo::Message::Severity severity = DocumentQmlInfo::Message::Hint;
        switch( message.kind ){
        case QmlJS::Severity::Hint: severity = DocumentQmlInfo::Message::Hint; break;
        case QmlJS::Severity::MaybeWarning: severity = DocumentQmlInfo::Message::MaybeWarning; break;
        case QmlJS::Severity::Warning: severity = DocumentQmlInfo::Message::Warning; break;
        case QmlJS::Severity::MaybeError: severity = DocumentQmlInfo::Message::MaybeError; break;
        case QmlJS::Severity::Error: severity = DocumentQmlInfo::Message::Error; break;
        }

        d->messages.append(
            DocumentQmlInfo::Message(
                severity, message.loc.offset, message.loc.startLine, message.message
            )
        );
    }

    return parseResult;
}

QmlJS::Bind *DocumentQmlInfo::internalBind(){
    Q_D(DocumentQmlInfo);
    return d->internalDocBind;
}

bool DocumentQmlInfo::isObject(const QString &typeString){
    if ( typeString == "bool" || typeString == "double" || typeString == "enumeration" ||
         typeString == "int" || typeString == "list" || typeString == "real" ||
         typeString == "string" || typeString == "url" || typeString == "var" )
        return false;
    return true;
}

QString DocumentQmlInfo::path() const{
    Q_D(const DocumentQmlInfo);
    return d->internalDoc->path();
}

QString DocumentQmlInfo::componentName() const{
    Q_D(const DocumentQmlInfo);
    return d->internalDoc->componentName();
}

DocumentQmlValueObjects::Ptr DocumentQmlInfo::createObjects() const{
    Q_D(const DocumentQmlInfo);
    DocumentQmlValueObjects::Ptr objects = DocumentQmlValueObjects::create();
    objects->visit(d->internalDoc->ast());
    return objects;
}

void DocumentQmlInfo::syncBindings(const QString &source, ProjectDocument *document, QObject *root){
    if ( document && document->hasBindings() ){
        DocumentQmlInfo::Ptr docinfo = DocumentQmlInfo::create(document->file()->path());
        docinfo->parse(source);

        DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();

        for ( ProjectDocument::BindingIterator it = document->bindingsBegin(); it != document->bindingsEnd(); ++it ){
            CodeRuntimeBinding* binding = *it;
            QQmlProperty foundProperty(findRuntimeProperty(objects->root(), root, binding->declaration(), source));
            if ( foundProperty.isValid() )
                foundProperty.connectNotifySignal(binding, SLOT(updateValue()));
        }
    }
}

void DocumentQmlInfo::syncBindings(
        const QString &source,
        ProjectDocument* document,
        QList<CodeRuntimeBinding *> bindings,
        QObject *root)
{
    if ( bindings.isEmpty() )
        return;

    DocumentQmlInfo::Ptr docinfo = DocumentQmlInfo::create(document->file()->path());
    docinfo->parse(source);

    DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();

    for ( QList<CodeRuntimeBinding*>::iterator it = bindings.begin(); it != bindings.end(); ++it) {
        CodeRuntimeBinding* binding = *it;
        QQmlProperty foundProperty(findRuntimeProperty(objects->root(), root, binding->declaration(), source));
        if ( foundProperty.isValid() )
            foundProperty.connectNotifySignal(binding, SLOT(updateValue()));
    }
}

QQmlProperty DocumentQmlInfo::findMatchingProperty(
        const QString &source,
        ProjectDocument *document,
        CodeDeclaration::Ptr declaration,
        QObject *root)
{
    DocumentQmlInfo::Ptr docinfo = DocumentQmlInfo::create(document->file()->path());
    docinfo->parse(source);

    DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();

    if ( objects->root() )
        return QQmlProperty(findRuntimeProperty(objects->root(), root, declaration, source));

    return QQmlProperty();
}

DocumentQmlInfo::~DocumentQmlInfo(){
}

}// namespace

