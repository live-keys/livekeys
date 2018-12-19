/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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
#include "live/qmldeclaration.h"
#include "live/projectdocument.h"
#include "live/projectfile.h"
#include "bindingchannel.h"

#include "qmljs/qmljsdocument.h"
#include "qmljs/qmljsbind.h"
#include "qmlidvisitor_p.h"
#include "documentqmlranges_p.h"

#include <QQmlProperty>
#include <QQmlListReference>

namespace lv{

namespace{

    BindingPath::Node* findDeclarationPathImpl(
        DocumentQmlValueObjects::RangeObject *object,
        QmlDeclaration::Ptr declaration)
    {
        int position = declaration->position();

        for ( int i = 0; i < object->properties.size(); ++i ){

            // found property start position
            if ( object->properties[i]->begin == position ){

                if ( declaration->identifierChain().isEmpty() )
                    return nullptr;

                // iterate property chain (eg. border.size => (border, size))
                BindingPath::Node* currentParent = nullptr;

                for ( int j = 0; j < declaration->identifierChain().size(); ++j ){
                    BindingPath::PropertyNode* n = new BindingPath::PropertyNode;
                    n->propertyName = declaration->identifierChain()[j];
                    n->objectName = object->properties[i]->object();
                    n->parent = currentParent;
                    if ( currentParent )
                        currentParent->child = n;
                    currentParent = n;
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

                BindingPath::Node* n = currentParent;
                while ( n->parent != nullptr )
                    n = n->parent;
                return n;

            } else if ( object->properties[i]->child &&
                        object->properties[i]->begin < position &&
                        object->properties[i]->end > position )
            {
                BindingPath::Node* currentParent = nullptr;

                QStringList propertyNames = object->properties[i]->name();

                for ( int j = 0; j < propertyNames.size(); ++j ){
                    BindingPath::PropertyNode* n = new BindingPath::PropertyNode;
                    n->propertyName = propertyNames[j];
                    n->objectName = object->properties[i]->object();
                    n->parent = currentParent;
                    if ( currentParent )
                        currentParent->child = n;
                    currentParent = n;
                }

                BindingPath::Node* n = findDeclarationPathImpl(
                    object->properties[i]->child, declaration
                );
                if ( !n ){
                    delete currentParent;
                    return nullptr;
                }

                n->parent = currentParent;
                currentParent->child = n;

                while ( n->parent != nullptr )
                    n = n->parent;
                return n;
            }
        } // properties end

        for ( int i = 0; i < object->children.size(); ++i ){

            if ( position > object->children[i]->begin &&
                 position < object->children[i]->end )
            {
                BindingPath::IndexNode* indexNode = new BindingPath::IndexNode;
                indexNode->index = i;

                BindingPath::Node* n = findDeclarationPathImpl(
                    object->children[i], declaration
                );

                if ( !n ){
                    delete indexNode;
                    return nullptr;
                }

                indexNode->child = n;
                n->parent = indexNode;

                return indexNode;

            } else if ( position == object->children[i]->begin ){ // found object

                // return the top property, and the index of the object
                declaration->setValuePositionOffset(0);
                declaration->setValueLength(object->children[i]->end - object->children[i]->begin);

                BindingPath::IndexNode* indexNode = new BindingPath::IndexNode;
                indexNode->index = i;

                return indexNode;
            }
        }

        return nullptr;
    }

    void traversePath(BindingPath* path, BindingPath::Node* n, QObject* object){
        if ( n == nullptr || object == nullptr)
            return;

        if ( n->type() == BindingPath::Node::Property ){
            BindingPath::PropertyNode* pn = static_cast<BindingPath::PropertyNode*>(n);

            QQmlProperty prop(object, pn->propertyName);
            if ( !prop.isValid() )
                return;

            if ( n->child == nullptr ){
                path->updateConnection(prop);
                return;
            } else {
                traversePath(path, n->child, prop.read().value<QObject*>());
            }
        } else if ( n->type() == BindingPath::Node::Index ){
            BindingPath::IndexNode* in = static_cast<BindingPath::IndexNode*>(n);

            QQmlProperty prop(object);
            if ( !prop.isValid() )
                return;
            if ( prop.propertyTypeCategory() == QQmlProperty::Object && in->index != 0 )
                return;

            if ( n->child == nullptr ){
                path->updateConnection(prop, in->index);
                return;
            } else {
                if ( prop.propertyTypeCategory() == QQmlProperty::Object ){
                    traversePath(path, n->child, prop.read().value<QObject*>());
                } else if ( prop.propertyTypeCategory() == QQmlProperty::List ){
                    QQmlListReference ppref = qvariant_cast<QQmlListReference>(prop.read());
                    if ( ppref.canAt() && ppref.canCount() && ppref.count() > in->index ){
                        traversePath(path, n->child, ppref.at(in->index));
                    } else
                        return;
                }
            }
        }
    }

} // namespace


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

void DocumentQmlInfo::extractTypeNameRange(const DocumentQmlInfo::ValueReference &valueref, int &begin, int &end){
    if ( isValueNull(valueref) || valueref.parent != this ){
        begin = -1;
        end = -1;
        return;
    }

    if ( const QmlJS::ASTObjectValue* vob = valueref.value->asAstObjectValue() ){
        if ( vob->typeName() ){
            begin = vob->typeName()->firstSourceLocation().begin();
            end = vob->typeName()->lastSourceLocation().end();
        }
    }
}

void DocumentQmlInfo::extractRange(const DocumentQmlInfo::ValueReference &valueref, int &begin, int &end){
    if ( isValueNull(valueref) || valueref.parent != this ){
        begin = -1;
        end = -1;
        return;
    }

    if ( const QmlJS::ASTObjectValue* vob = valueref.value->asAstObjectValue() ){
        if ( vob->initializer() ){
            begin = vob->initializer()->firstSourceLocation().begin();
            end = vob->initializer()->lastSourceLocation().end();
        }
    }
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

const DocumentQmlInfo::ASTReference DocumentQmlInfo::astObjectAtPosition(int position){
    Q_D(const DocumentQmlInfo);
    DocumentQmlRanges::Range range = d->ranges.findClosestRange(position);
    return DocumentQmlInfo::ASTReference(range.ast);
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

DocumentQmlValueObjects::Ptr DocumentQmlInfo::createObjects(const DocumentQmlInfo::ASTReference &ast) const{
    DocumentQmlValueObjects::Ptr objects = DocumentQmlValueObjects::create();
    if ( ast.node ){
        objects->visit(ast.node);
    }
    return objects;
}

void DocumentQmlInfo::traverseBindingPath(BindingPath *path, QObject *root){
    traversePath(path, path->root(), root);
}

BindingPath *DocumentQmlInfo::findDeclarationPath(
        DocumentQmlValueObjects::RangeObject *root,
        QmlDeclaration::Ptr declaration)
{
    if ( !root )
        return nullptr;

    BindingPath::Node* n = findDeclarationPathImpl(root, declaration);
    if ( !n )
        return nullptr;

    BindingPath* path = new BindingPath;
    path->updatePath(n);

    return path;
}

BindingPath* DocumentQmlInfo::findDeclarationPath(
        const QString &source,
        ProjectDocument *document,
        QmlDeclaration::Ptr declaration)
{
    DocumentQmlInfo::Ptr docinfo = DocumentQmlInfo::create(document->file()->path());
    docinfo->parse(source);

    DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();
    return findDeclarationPath(objects->root(), declaration);
}

DocumentQmlInfo::~DocumentQmlInfo(){
}

}// namespace

