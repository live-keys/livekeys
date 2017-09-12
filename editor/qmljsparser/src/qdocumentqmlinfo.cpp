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

#include "qdocumentqmlinfo.h"
#include "qmljs/qmljsdocument.h"
#include "qmljs/qmljsbind.h"
#include "qqmlidvisitor_p.h"
#include "qdocumentqmlranges_p.h"
#include "qdocumentqmlvalueobjects.h"
#include "qcodedeclaration.h"
#include "qcoderuntimebinding.h"

#include "qprojectdocument.h"
#include "qprojectfile.h"

#include <QQmlProperty>
#include <QQmlListReference>

namespace lcv{

namespace{

    QQmlProperty findRuntimeProperty(
        QDocumentQmlValueObjects::RangeObject *object,
        QObject *root,
        QCodeDeclaration::Ptr declaration,
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


class QDocumentQmlInfoPrivate{
public:
    QmlJS::Document::MutablePtr      internalDoc;
    QmlJS::Bind*                     internalDocBind;
    QDocumentQmlRanges               ranges;
    QList<QDocumentQmlInfo::Message> messages;
};

QDocumentQmlInfo::Dialect QDocumentQmlInfo::extensionToDialect(const QString &extension){
    static QHash<QString, QDocumentQmlInfo::Dialect> map;
    map["js"]         = QDocumentQmlInfo::Javascript;
    map["qml"]        = QDocumentQmlInfo::Qml;
    map["qmltypes"]   = QDocumentQmlInfo::QmlTypeInfo;
    map["qmlproject"] = QDocumentQmlInfo::QmlProject;
    map["json"]       = QDocumentQmlInfo::Json;

    return map.value(extension, QDocumentQmlInfo::Unknown);
}

QDocumentQmlInfo::QDocumentQmlInfo(const QString &fileName)
    : d_ptr(new QDocumentQmlInfoPrivate)
{
    Q_D(QDocumentQmlInfo);
    QDocumentQmlInfo::Dialect dialect = extensionToDialect(QFileInfo(fileName).suffix());
    if ( dialect == QDocumentQmlInfo::Javascript )
        d->internalDoc = QmlJS::Document::create(fileName, QmlJS::Dialect::JavaScript);
    else if ( dialect == QDocumentQmlInfo::Qml || fileName == "" ){
        d->internalDoc = QmlJS::Document::create(fileName, QmlJS::Dialect::Qml);
    } else
        d->internalDoc = QmlJS::Document::create(fileName, QmlJS::Dialect::NoLanguage);
}

QDocumentQmlInfo::Ptr QDocumentQmlInfo::create(const QString &fileName){
    return QDocumentQmlInfo::Ptr(new QDocumentQmlInfo(fileName));
}

QStringList QDocumentQmlInfo::extractIds() const{
    Q_D(const QDocumentQmlInfo);
    if ( d->internalDocBind->idEnvironment() == 0 )
        return QStringList();

    QIdExtractor extractor;
    d->internalDocBind->idEnvironment()->processMembers(&extractor);
    return extractor.ids();
}

const QDocumentQmlInfo::ValueReference QDocumentQmlInfo::rootObject(){
    Q_D(QDocumentQmlInfo);
    return QDocumentQmlInfo::ValueReference(d->internalDocBind->rootObjectValue(), this);
}

const QDocumentQmlInfo::ValueReference QDocumentQmlInfo::valueForId(const QString &id) const{
    Q_D(const QDocumentQmlInfo);
    if ( d->internalDocBind->idEnvironment() == 0 )
        return QDocumentQmlInfo::ValueReference();

    QIdValueExtractor valueExtractor(id);
    d->internalDocBind->idEnvironment()->processMembers(&valueExtractor);
    return QDocumentQmlInfo::ValueReference(valueExtractor.value(), this);
}

QDocumentQmlObject QDocumentQmlInfo::extractValueObject(
        const ValueReference &valueref,
        ValueReference *parent) const
{
    QDocumentQmlObject vodata;
    if ( isValueNull(valueref) || valueref.parent != this )
        return vodata;

    if ( const QmlJS::ASTObjectValue* vob = valueref.value->asAstObjectValue() ){
        if ( vob->typeName() )
            vodata.setTypeName(vob->typeName()->name.toString());

        QValueMemberExtractor extractor(&vodata);
        vob->processMembers(&extractor);
        if ( parent ){
            parent->value = extractor.parent();
            parent->parent = this;
        }
    }

    return vodata;
 }

QString QDocumentQmlInfo::extractTypeName(const QDocumentQmlInfo::ValueReference &valueref) const{
    if ( isValueNull(valueref) || valueref.parent != this )
        return "";

    if ( const QmlJS::ASTObjectValue* vob = valueref.value->asAstObjectValue() )
        if ( vob->typeName() )
            return vob->typeName()->name.toString();

    return "";
}

void QDocumentQmlInfo::createRanges(){
    Q_D(QDocumentQmlInfo);
    d->ranges(d->internalDoc);
}

const QDocumentQmlInfo::ValueReference QDocumentQmlInfo::valueAtPosition(int position) const{
    Q_D(const QDocumentQmlInfo);

    QDocumentQmlRanges::Range range = d->ranges.findClosestRange(position);
    if ( range.ast == 0 )
        return QDocumentQmlInfo::ValueReference();

    QmlJS::ObjectValue* value = d->internalDocBind->findQmlObject(range.ast);
    return QDocumentQmlInfo::ValueReference(value, this);
}

const QDocumentQmlInfo::ValueReference QDocumentQmlInfo::valueAtPosition(
        int position,
        int &begin,
        int &end) const
{
    Q_D(const QDocumentQmlInfo);

    QList<QDocumentQmlRanges::Range> rangePath = d->ranges.findRangePath(position);
    foreach( const QDocumentQmlRanges::Range& range, rangePath ){
        begin = range.begin;
        end   = range.end;

//        qDebug() << begin << end;
//        QmlJS::ObjectValue* value = d->internalDocBind->findQmlObject(range.ast);
//        return QDocumentQmlInfo::ValueReference(value, this);
    }

    QDocumentQmlRanges::Range range = d->ranges.findClosestRange(position);
    if ( range.ast == 0 )
        return QDocumentQmlInfo::ValueReference();

    begin = range.begin;
    end   = range.end;

    QmlJS::ObjectValue* value = d->internalDocBind->findQmlObject(range.ast);
    return QDocumentQmlInfo::ValueReference(value, this);
}

bool QDocumentQmlInfo::isValueNull(const QDocumentQmlInfo::ValueReference& vr) const{
    return vr.value == 0;
}

bool QDocumentQmlInfo::isParsedCorrectly() const{
    Q_D(const QDocumentQmlInfo);
    return d->internalDoc->isParsedCorrectly();
}

bool QDocumentQmlInfo::parse(const QString &source){
    Q_D(QDocumentQmlInfo);
    d->messages.clear();
    d->internalDoc->setSource(source);
    bool parseResult = d->internalDoc->parse();
    d->internalDocBind = d->internalDoc->bind();

    foreach( const QmlJS::DiagnosticMessage& message, d->internalDoc->diagnosticMessages() ){
        QDocumentQmlInfo::Message::Severity severity = QDocumentQmlInfo::Message::Hint;
        switch( message.kind ){
        case QmlJS::Severity::Hint: severity = QDocumentQmlInfo::Message::Hint; break;
        case QmlJS::Severity::MaybeWarning: severity = QDocumentQmlInfo::Message::MaybeWarning; break;
        case QmlJS::Severity::Warning: severity = QDocumentQmlInfo::Message::Warning; break;
        case QmlJS::Severity::MaybeError: severity = QDocumentQmlInfo::Message::MaybeError; break;
        case QmlJS::Severity::Error: severity = QDocumentQmlInfo::Message::Error; break;
        }

        d->messages.append(
            QDocumentQmlInfo::Message(
                severity, message.loc.offset, message.loc.startLine, message.message
            )
        );
    }

    return parseResult;
}

QmlJS::Bind *QDocumentQmlInfo::internalBind(){
    Q_D(QDocumentQmlInfo);
    return d->internalDocBind;
}

bool QDocumentQmlInfo::isObject(const QString &typeString){
    if ( typeString == "bool" || typeString == "double" || typeString == "enumeration" ||
         typeString == "int" || typeString == "list" || typeString == "real" ||
         typeString == "string" || typeString == "url" || typeString == "var" )
        return false;
    return true;
}

QString QDocumentQmlInfo::path() const{
    Q_D(const QDocumentQmlInfo);
    return d->internalDoc->path();
}

QString QDocumentQmlInfo::componentName() const{
    Q_D(const QDocumentQmlInfo);
    return d->internalDoc->componentName();
}

QDocumentQmlValueObjects::Ptr QDocumentQmlInfo::createObjects() const{
    Q_D(const QDocumentQmlInfo);
    QDocumentQmlValueObjects::Ptr objects = QDocumentQmlValueObjects::create();
    objects->visit(d->internalDoc->ast());
    return objects;
}

void QDocumentQmlInfo::syncBindings(const QString &source, QProjectDocument *document, QObject *root){
    if ( document && document->hasBindings() ){
        QDocumentQmlInfo::Ptr docinfo = QDocumentQmlInfo::create(document->file()->path());
        docinfo->parse(source);

        QDocumentQmlValueObjects::Ptr objects = docinfo->createObjects();

        for ( QProjectDocument::BindingIterator it = document->bindingsBegin(); it != document->bindingsEnd(); ++it ){
            QCodeRuntimeBinding* binding = *it;
            QQmlProperty foundProperty(findRuntimeProperty(objects->root(), root, binding->declaration(), source));
            if ( foundProperty.isValid() )
                foundProperty.connectNotifySignal(binding, SLOT(updateValue()));
        }
    }
}

void QDocumentQmlInfo::syncBindings(
        const QString &source,
        QProjectDocument* document,
        QList<QCodeRuntimeBinding *> bindings,
        QObject *root)
{
    if ( bindings.isEmpty() )
        return;

    QDocumentQmlInfo::Ptr docinfo = QDocumentQmlInfo::create(document->file()->path());
    docinfo->parse(source);

    QDocumentQmlValueObjects::Ptr objects = docinfo->createObjects();

    for ( QList<QCodeRuntimeBinding*>::iterator it = bindings.begin(); it != bindings.end(); ++it) {
        QCodeRuntimeBinding* binding = *it;
        QQmlProperty foundProperty(findRuntimeProperty(objects->root(), root, binding->declaration(), source));
        if ( foundProperty.isValid() )
            foundProperty.connectNotifySignal(binding, SLOT(updateValue()));
    }
}

QQmlProperty QDocumentQmlInfo::findMatchingProperty(
        const QString &source,
        QProjectDocument *document,
        QCodeDeclaration::Ptr declaration,
        QObject *root)
{
    QDocumentQmlInfo::Ptr docinfo = QDocumentQmlInfo::create(document->file()->path());
    docinfo->parse(source);

    QDocumentQmlValueObjects::Ptr objects = docinfo->createObjects();

    if ( objects->root() )
        return QQmlProperty(findRuntimeProperty(objects->root(), root, declaration, source));

    return QQmlProperty();
}

QDocumentQmlInfo::~QDocumentQmlInfo(){
}

}// namespace

