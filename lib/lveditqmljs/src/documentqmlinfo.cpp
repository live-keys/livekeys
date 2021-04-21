/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
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
#include "live/visuallogqt.h"
#include "live/hookcontainer.h"
#include "qmlwatcher.h"
#include "qmlbindingchannel.h"

#include "qmljs/qmljsdocument.h"
#include "qmljs/qmljsbind.h"
#include "qmlidvisitor_p.h"
#include "documentqmlranges_p.h"

#include <QQmlContext>
#include <QQmlProperty>
#include <QFileInfo>
#include <QQmlListReference>

namespace lv{

namespace{


    class TraversalNodeResult{
    public:
        TraversalNodeResult(QmlBindingPath::Node* pBindingNode = nullptr, DocumentQmlValueObjects::RangeItem* pRange = nullptr)
            : bindingNode(pBindingNode), range(pRange)
        {}

        QmlBindingPath::Node* bindingNode;
        DocumentQmlValueObjects::RangeItem* range;
    };

    /// \private
    TraversalNodeResult findDeclarationPathImpl(
        DocumentQmlValueObjects::RangeObject *object,
        QmlDeclaration::Ptr declaration)
    {
        int position = declaration->position();

        for ( int i = 0; i < object->properties.size(); ++i ){

            // found property start position
            if ( object->properties[i]->begin == position ){

                if ( declaration->identifierChain().isEmpty() )
                    return TraversalNodeResult();

                // iterate property chain (eg. border.size => (border, size))
                QmlBindingPath::Node* currentParent = nullptr;

                for ( int j = 0; j < declaration->identifierChain().size(); ++j ){
                    QmlBindingPath::PropertyNode* n = new QmlBindingPath::PropertyNode;
                    n->propertyName = declaration->identifierChain()[j];
                    n->objectName = object->properties[i]->object();
                    n->parent = currentParent;
                    if ( currentParent )
                        currentParent->child = n;
                    currentParent = n;
                }

                declaration->setValuePositionOffset(
                    object->properties[i]->valueBegin -
                    object->properties[i]->begin -
                    declaration->identifierLength()
                );

                if ( object->properties[i]->child ){
                    declaration->setValueObjectScopeOffset(object->properties[i]->child->identifierEnd - declaration->valuePosition());
                }

                declaration->setValueLength(
                    object->properties[i]->end - object->properties[i]->valueBegin
                );

                QmlBindingPath::Node* n = currentParent;
                while ( n->parent != nullptr )
                    n = n->parent;

                TraversalNodeResult dpr;
                dpr.bindingNode = n;
                dpr.range = object->properties[i];

                return dpr;

            } else if ( object->properties[i]->child &&
                        object->properties[i]->begin < position &&
                        object->properties[i]->end > position )
            {
                QmlBindingPath::Node* currentParent = nullptr;

                QStringList propertyNames = object->properties[i]->name();

                for ( int j = 0; j < propertyNames.size(); ++j ){
                    QmlBindingPath::PropertyNode* n = new QmlBindingPath::PropertyNode;
                    n->propertyName = propertyNames[j];
                    n->objectName = object->properties[i]->object();
                    n->parent = currentParent;
                    if ( currentParent )
                        currentParent->child = n;
                    currentParent = n;
                }

                TraversalNodeResult dpr = findDeclarationPathImpl(
                    object->properties[i]->child, declaration
                );
                if ( !dpr.bindingNode ){
                    delete currentParent;
                    return TraversalNodeResult();
                }

                dpr.bindingNode->parent = currentParent;
                currentParent->child = dpr.bindingNode;

                while ( dpr.bindingNode->parent != nullptr )
                    dpr.bindingNode = dpr.bindingNode->parent;
                return dpr;
            }
        } // properties end

        for ( int i = 0; i < object->children.size(); ++i ){

            if ( position > object->children[i]->begin &&
                 position < object->children[i]->end )
            {
                QmlBindingPath::IndexNode* indexNode = new QmlBindingPath::IndexNode;
                indexNode->index = i;

                TraversalNodeResult dpr = findDeclarationPathImpl(
                    object->children[i], declaration
                );

                if ( !dpr.bindingNode ){
                    delete indexNode;
                    return TraversalNodeResult();
                }

                indexNode->child = dpr.bindingNode;
                dpr.bindingNode->parent = indexNode;

                dpr.bindingNode = indexNode;
                return dpr;

            } else if ( position == object->children[i]->begin ){ // found object

                // return the top property, and the index of the object
                declaration->setValuePositionOffset(0);
                declaration->setValueLength(object->children[i]->end - object->children[i]->begin);

                QmlBindingPath::IndexNode* indexNode = new QmlBindingPath::IndexNode;
                indexNode->index = i;

                TraversalNodeResult dpr;
                dpr.bindingNode = indexNode;
                dpr.range = object->children[i];
                return dpr;
            }
        }

        return TraversalNodeResult();
    }

} // namespace

// class DocumentQmlInfoPrivate
// ------------------------------------------------------------------------------

/// \private
class DocumentQmlInfoPrivate{
public:
    QmlJS::Document::MutablePtr     internalDoc;
    QmlJS::Bind*                    internalDocBind;
    DocumentQmlInfo::ImportList     imports;
    DocumentQmlRanges               ranges;
    QList<DocumentQmlInfo::Message> messages;
};

/**
 * \class lv::DocumentQmlInfo::ValueReference
 * \ingroup lveditqmljs
 * \brief Contains a reference to a value within a parsed qml document
 */

/**
 * \property lv::DocumentQmlInfo::ValueReference::value
 * \brief Internal reference to the value
 */

/**
 * \property lv::DocumentQmlInfo::ValueReference::parent
 * \brief Reference to the parent
 */

/**
 * \class lv::DocumentQmlInfo::ASTReference
 * \ingroup lveditqmljs
 * \brief Reference to an AST node within the parsed qml document
 */

/**
 * \property lv::DocumentQmlInfo::ASTReference::node
 * \brief Internal reference to the AST node
 */


/**
 * \class lv::DocumentQmlInfo::Message
 * \ingroup lveditqmljs
 * \brief Message information containing parsing status.
 */


/**
 * \fn lv::DocumentQmlInfo::Message::Message()
 * \brief DocumentQmlInfo::Message constructor
 */

/**
 * \property lv::DocumentQmlInfo::Message::kind
 * \brief Message severity level
 */

/**
 * \property lv::DocumentQmlInfo::Message::position
 * \brief Position the message was generated at
 */

/**
 * \property lv::DocumentQmlInfo::Message::line
 * \brief Line the message was generated at
 */

/**
 * \property lv::DocumentQmlInfo::Message::text
 * \brief Message text
 */

// class DocumentQmlInfo
// ------------------------------------------------------------------------------

/**
 * \class lv::DocumentQmlInfo
 * \ingroup lveditqmljs
 * \brief Contains parsed information about a Project qml document
 */


/**
 * \brief Returns the lv::DocumentQmlInfo::Dialect according to the file \p extension
 */
DocumentQmlInfo::Dialect DocumentQmlInfo::extensionToDialect(const QString &extension){
    QHash<QString, DocumentQmlInfo::Dialect> map;
    map["js"]         = DocumentQmlInfo::Javascript;
    map["qml"]        = DocumentQmlInfo::Qml;
    map["qmltypes"]   = DocumentQmlInfo::QmlTypeInfo;
    map["qmlproject"] = DocumentQmlInfo::QmlProject;
    map["json"]       = DocumentQmlInfo::Json;

    return map.value(extension, DocumentQmlInfo::Unknown);
}

/**
 * \brief DocumentQmlInfo constructor.
 *
 * This constructor is private. Use lv::DocumentQmlInfo::create() function.
 */
DocumentQmlInfo::DocumentQmlInfo(const QString &fileName)
    : d_ptr(new DocumentQmlInfoPrivate)
{
    Q_D(DocumentQmlInfo);

    QString suffix = fileName.indexOf("T:") != -1 ? "qml" : QFileInfo(fileName).suffix();

    DocumentQmlInfo::Dialect dialect = extensionToDialect(suffix);
    if ( dialect == DocumentQmlInfo::Javascript )
        d->internalDoc = QmlJS::Document::create(fileName, QmlJS::Dialect::JavaScript);
    else if ( dialect == DocumentQmlInfo::Qml || fileName == "" ){
        d->internalDoc = QmlJS::Document::create(fileName, QmlJS::Dialect::Qml);
    } else
        d->internalDoc = QmlJS::Document::create(fileName, QmlJS::Dialect::NoLanguage);
}

/**
 * \brief Constructs a new lv::DocumentQmlInfo object.
 */
DocumentQmlInfo::Ptr DocumentQmlInfo::create(const QString &fileName){
    return DocumentQmlInfo::Ptr(new DocumentQmlInfo(fileName));
}

DocumentQmlInfo::Ptr DocumentQmlInfo::createAndParse(const QString &fileName, const QString &source){
    DocumentQmlInfo::Ptr r = create(fileName);
    r->parse(source);
    return r;
}

/**
 * \brief Extract the declared id's within a qml document.
 */
QStringList DocumentQmlInfo::extractIds() const{
    Q_D(const DocumentQmlInfo);
    if ( d->internalDocBind->idEnvironment() == nullptr )
        return QStringList();

    IdExtractor extractor;
    d->internalDocBind->idEnvironment()->processMembers(&extractor);
    return extractor.ids();
}

/**
 * \brief Return a reference to the root object of this qml document
 */
const DocumentQmlInfo::ValueReference DocumentQmlInfo::rootObject(){
    Q_D(DocumentQmlInfo);
    return DocumentQmlInfo::ValueReference(d->internalDocBind->rootObjectValue(), this);
}

/**
 * \brief Returns a reference to the value represented by the given id.
 */
const DocumentQmlInfo::ValueReference DocumentQmlInfo::valueForId(const QString &id) const{
    Q_D(const DocumentQmlInfo);
    if ( d->internalDocBind->idEnvironment() == nullptr )
        return DocumentQmlInfo::ValueReference();

    IdValueExtractor valueExtractor(id);
    d->internalDocBind->idEnvironment()->processMembers(&valueExtractor);
    return DocumentQmlInfo::ValueReference(valueExtractor.value(), this);
}


/**
 * \brief Extracts a usable object given a value reference.
 *
 * For example, you can use this class together with lv::DocumentQmlInfo::valueForId() to inspect
 * a specific object defined with an id within the qml document.
 */
lv::QmlTypeInfo::Ptr DocumentQmlInfo::extractValueObject(
        const DocumentQmlInfo::ValueReference &valueref, DocumentQmlInfo::ValueReference *parent) const
{
    lv::QmlTypeInfo::Ptr vodata = lv::QmlTypeInfo::create();
    if ( isValueNull(valueref) || valueref.parent != this )
        return vodata;

    if ( const QmlJS::ASTObjectValue* vob = valueref.value->asAstObjectValue() ){

        ValueMemberExtractor extractor(vodata);
        vob->processMembers(&extractor);
        if ( parent ){
            parent->value = extractor.parent();
            parent->parent = this;
        }

        if ( vob->typeName() )
            vodata->setExportType(QmlTypeReference(QmlTypeReference::Qml, vob->typeName()->name.toString()));
    }

    return vodata;
}

QmlTypeInfo::Ptr DocumentQmlInfo::extractValueObjectWithExport(
        const DocumentQmlInfo::ValueReference &valueref,
        const QString &componentName,
        const QString &libraryPath) const
{
    lv::QmlTypeInfo::Ptr vodata = lv::QmlTypeInfo::create();
    if ( isValueNull(valueref) || valueref.parent != this )
        return vodata;

    if ( const QmlJS::ASTObjectValue* vob = valueref.value->asAstObjectValue() ){

        ValueMemberExtractor extractor(vodata);
        vob->processMembers(&extractor);
        vodata->setExportType(QmlTypeReference(QmlTypeReference::Qml, componentName, libraryPath));

        if ( vob->typeName() )
            vodata->setInheritanceType(QmlTypeReference(QmlTypeReference::Unknown, vob->typeName()->name.toString()));
    }

    return vodata;
}

/**
 * \brief Extract the name of the type given by this value reference.
 */
QStringList DocumentQmlInfo::extractTypeName(const DocumentQmlInfo::ValueReference &valueref) const{
    if ( isValueNull(valueref) || valueref.parent != this )
        return QStringList();

    QStringList result;
    if ( const QmlJS::ASTObjectValue* vob = valueref.value->asAstObjectValue() ){
        if ( vob->typeName() ){
            result << vob->typeName()->name.toString();

            if ( vob->typeName()->next )
                result << vob->typeName()->next->name.toString();
        }
    }

    return result;
}

/**
 * \brief Extract the range of the type given by a value reference
 *
 * \p begin and \p end will be populated with the given range, or -1 if the range cannot be extracted
 */
void DocumentQmlInfo::extractTypeNameRange(const DocumentQmlInfo::ValueReference &valueref, int &begin, int &end) const{
    if ( isValueNull(valueref) || valueref.parent != this ){
        begin = -1;
        end = -1;
        return;
    }

    if ( const QmlJS::ASTObjectValue* vob = valueref.value->asAstObjectValue() ){
        if ( vob->typeName() ){
            begin = static_cast<int>(vob->typeName()->firstSourceLocation().begin());
            end = static_cast<int>(vob->typeName()->lastSourceLocation().end());
        }
    }
}

/**
 * \brief Extract the full range given by a value reference
 *
 * \p begin and \p end will be populated with the given range, or -1 if the range cannot be extracted
 */
void DocumentQmlInfo::extractRange(const DocumentQmlInfo::ValueReference &valueref, int &begin, int &end){
    if ( isValueNull(valueref) || valueref.parent != this ){
        begin = -1;
        end = -1;
        return;
    }

    if ( const QmlJS::ASTObjectValue* vob = valueref.value->asAstObjectValue() ){
        if ( vob->initializer() ){
            begin = static_cast<int>(vob->initializer()->firstSourceLocation().begin());
            end = static_cast<int>(vob->initializer()->lastSourceLocation().end());
        }
    }
}

/**
 * \brief Finds the ranges within this document
 */
void DocumentQmlInfo::createRanges(){
    Q_D(DocumentQmlInfo);
    d->ranges(d->internalDoc);
}

/**
 * \brief Find the value reference at a given \p position
 */
const DocumentQmlInfo::ValueReference DocumentQmlInfo::valueAtPosition(int position) const{
    Q_D(const DocumentQmlInfo);

    DocumentQmlRanges::Range range = d->ranges.findClosestRange(position);
    if ( range.ast == nullptr )
        return DocumentQmlInfo::ValueReference();

    QmlJS::ObjectValue* value = d->internalDocBind->findQmlObject(range.ast);
    return DocumentQmlInfo::ValueReference(value, this);
}

/**
 * \brief Find the value reference and range at a given \p position
 *
 * The \p begin and \p end arguments will be populated with the range positino
 */
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
    if ( range.ast == nullptr )
        return DocumentQmlInfo::ValueReference();

    begin = range.begin;
    end   = range.end;

    QmlJS::ObjectValue* value = d->internalDocBind->findQmlObject(range.ast);
    return DocumentQmlInfo::ValueReference(value, this);
}

/**
 * \brief Returns the reference to an AST node at a specified \p position
 */
const DocumentQmlInfo::ASTReference DocumentQmlInfo::astObjectAtPosition(int position){
    Q_D(const DocumentQmlInfo);
    DocumentQmlRanges::Range range = d->ranges.findClosestRange(position);
    return DocumentQmlInfo::ASTReference(range.ast);
}

QString DocumentQmlInfo::propertySourceFromObjectId(const QString &componentId, const QString &propertyName){
    Q_D(const DocumentQmlInfo);

    DocumentQmlInfo::ValueReference vr = valueForId(componentId);

    int begin, end;
    extractRange(vr, begin, end);

    if ( begin > -1 && end > 0){

        QString sourceData = "Worker" + d->internalDoc->source().mid(begin, end - begin);

        lv::DocumentQmlInfo::Ptr docinfo = lv::DocumentQmlInfo::create("Worker.qml");

        if ( !docinfo->parse(sourceData) )
            return "";

        lv::DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();
        for ( auto pit = objects->root()->properties.begin(); pit != objects->root()->properties.end(); ++pit ){
            lv::DocumentQmlValueObjects::RangeProperty* p = *pit;
            QString propertyName = sourceData.mid(p->begin, p->propertyEnd - p->begin);
            if ( propertyName == "run" ){
                QString propertyContent = sourceData.mid(p->valueBegin, p->end - p->valueBegin);
                return propertyContent;
            }
        }
    }
    return "";
}

/**
 * \brief Check wether the value reference is null.
 *
 * \returns true if \p vr is null, false otherwise
 */
bool DocumentQmlInfo::isValueNull(const DocumentQmlInfo::ValueReference& vr) const{
    return vr.value == nullptr;
}

/**
 * \brief Check wether the document was parsed correctly after calling the lv::DocumentQmlInfo::parse() method
 * \returns true if it was, false otherwise
 */
bool DocumentQmlInfo::isParsedCorrectly() const{
    Q_D(const DocumentQmlInfo);
    return d->internalDoc->isParsedCorrectly();
}

/**
 * \brief Parses the source code to an AST form
 * \returns True if the parse went correctly, false otherwise
 */
bool DocumentQmlInfo::parse(const QString &source){
    Q_D(DocumentQmlInfo);
    d->messages.clear();
    d->internalDoc->setSource(source);
    bool parseResult = d->internalDoc->parse();
    d->internalDocBind = d->internalDoc->bind();

    foreach( const QmlJS::DiagnosticMessage& message, d->internalDoc->diagnosticMessages() ){
        DocumentQmlInfo::Message::Severity severity = DocumentQmlInfo::Message::Hint;
        switch( message.kind ){
        case QmlJS::DiagnosticMessage::Error: severity = DocumentQmlInfo::Message::Error; break;
        case QmlJS::DiagnosticMessage::Warning: severity = DocumentQmlInfo::Message::Warning; break;
        }

        d->messages.append(
            DocumentQmlInfo::Message(
                severity, static_cast<int>(message.loc.offset), static_cast<int>(message.loc.startLine), message.message
            )
        );
    }

    d->imports = extractImports();

    return parseResult;
}

const QList<lv::DocumentQmlInfo::Message> &DocumentQmlInfo::diagnostics() const{
    Q_D(const DocumentQmlInfo);
    return d->messages;
}

/**
 * \brief Return a pointer to the internal QmlJS::Bind object
 */
QmlJS::Bind *DocumentQmlInfo::internalBind(){
    Q_D(DocumentQmlInfo);
    return d->internalDocBind;
}

QList<DocumentQmlInfo::Import> DocumentQmlInfo::extractImports(){
    QList<DocumentQmlInfo::Import> imports;
    QList<QmlJS::ImportInfo> importInfos = internalBind()->imports();

    for ( QList<QmlJS::ImportInfo>::iterator it = importInfos.begin(); it != importInfos.end(); ++it ){
        QmlJS::ImportKey impKey(*it);
        Import::Type importType;
        switch(impKey.type){
        case QmlJS::ImportType::Invalid:           importType = Import::Invalid; break;
        case QmlJS::ImportType::Library:           importType = Import::Library; break;
        case QmlJS::ImportType::Directory:         importType = Import::Directory; break;
        case QmlJS::ImportType::ImplicitDirectory: importType = Import::ImplicitDirectory; break;
        case QmlJS::ImportType::File:              importType = Import::File; break;
        case QmlJS::ImportType::UnknownFile:       importType = Import::UnknownFile; break;
        default:                                   importType = Import::Invalid;
        }

        imports << Import(
           importType,
           (importType == Import::Library ? it->name() : it->path()),
           it->name(),
           it->as(),
           it->version().majorVersion(),
           it->version().minorVersion(),
           it->ast()
                ? Document::Location(static_cast<int>(it->ast()->importToken.startLine), static_cast<int>(it->ast()->importToken.startColumn))
                : Document::Location()

       );
    }
    return imports;
}

/**
 * \brief Return the path of the Document
 */
QString DocumentQmlInfo::path() const{
    Q_D(const DocumentQmlInfo);
    return d->internalDoc->path();
}

/**
 * \brief Return the component name represented by this Document
 */
QString DocumentQmlInfo::componentName() const{
    Q_D(const DocumentQmlInfo);
    return d->internalDoc->componentName();
}

QString DocumentQmlInfo::source() const{
    Q_D(const DocumentQmlInfo);
    return d->internalDoc->source();
}

/**
 * \brief Tries to extract imports from a document that was not correcly parsed
 */
void DocumentQmlInfo::tryExtractImports(){
    Q_D(DocumentQmlInfo);
    if ( d->imports.isEmpty() ){
        QList<DocumentQmlInfo::Import> result;

        QString content = d->internalDoc->source();
        auto lines = content.split('\n');
        QList<std::pair<QStringList, int>> mid;

        for (int i = 0; i < lines.size(); ++i)
        {
            if (lines[i].length() == 0)
                continue;
            QStringList fragments = lines[i].split(';');
            for (auto fragment: fragments){
                auto parts = fragment.split(' ',  QString::SkipEmptyParts);

                if (parts.size() != 3 && parts.size() != 5)
                    return;

                if (parts[0] != "import")
                    return;

                if (parts.size() == 5 && parts[3] != "as")
                    return;

                auto p = std::make_pair(QStringList(), i);

                p.first.push_back(parts[1]);
                p.first.push_back(parts[2]);
                p.first.push_back(parts.size() == 5 ? parts[4] : "");

                mid.push_back(p);
            }
        }

        for (auto p: mid){
            QString major = "1"; QString minor = "0";
            QStringList version = p.first[1].split(".");
            if ( version.length() == 2 ){
                major = version[0];
                minor = version[1];
            }

            d->imports.append(DocumentQmlInfo::Import(
               DocumentQmlInfo::Import::Library,
               p.first[0], // path or name
               p.first[0], //name
               p.first[2], // as
               major.toInt(),
               minor.toInt(),
               Document::Location(p.second, 0)
           ));
        }
    }
}

/**
 * \brief Visit the AST and create the objects defined in this document
 * \returns A pointer to the lv::DocumentQmlValueObjects
 */
DocumentQmlValueObjects::Ptr DocumentQmlInfo::createObjects() const{
    Q_D(const DocumentQmlInfo);

    DocumentQmlValueObjects::Ptr objects = DocumentQmlValueObjects::create();
    objects->visit(d->internalDoc->ast());
    return objects;
}

/**
 * \brief Visit the AST from the given \p ast node and create the objects defined within
 * \returns A pointer to the lv::DocumentQmlValueObjects
 */
DocumentQmlValueObjects::Ptr DocumentQmlInfo::createObjects(const DocumentQmlInfo::ASTReference &ast) const{
    DocumentQmlValueObjects::Ptr objects = DocumentQmlValueObjects::create();
    if ( ast.node ){
        objects->visit(ast.node);
    }
    return objects;
}

/**
 * \brief Finds the binding path associated with a declaration within a range object
 *
 * To call this method, parse the document first, then get the values:
 *
 * \code
 * DocumentQmlInfo::Ptr docinfo = DocumentQmlInfo::create(document->file()->path());
 * docinfo->parse(source);
 * DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();
 * DocumentQmlInfo::findDeclarationPath(document, objects->root(), declaration);
 * \endcode
 *
 * \returns The found binding path on success, nullptr otherwise
 */
DocumentQmlInfo::TraversalResult DocumentQmlInfo::findDeclarationPath(
        ProjectDocument* document,
        DocumentQmlValueObjects::RangeObject *root,
        QmlDeclaration::Ptr declaration)
{
    if ( !root )
        return DocumentQmlInfo::TraversalResult();


    DocumentQmlInfo::TraversalResult tr;
    DocumentQmlValueObjects::RangeItem* range = nullptr;
    QmlBindingPath::Node* n = nullptr;

    if ( root->begin == declaration->position() ){ // cover the case of root being the actual object
        declaration->setValuePositionOffset(0);
        declaration->setValueLength(root->end - root->begin);

        QmlBindingPath::ComponentNode* componentNode = new QmlBindingPath::ComponentNode;
        componentNode->name = document->file()->name();
        n = componentNode;
        range = root;
    } else {
        TraversalNodeResult result = findDeclarationPathImpl(root, declaration);
        if ( result.bindingNode ){
            QmlBindingPath::ComponentNode* componentNode = new QmlBindingPath::ComponentNode;
            componentNode->name = document->file()->name();
            componentNode->child = result.bindingNode;
            result.bindingNode->parent = componentNode;
            n = componentNode;
            range = result.range;
        }
    }

    if ( !n )
        return DocumentQmlInfo::TraversalResult();

    QmlBindingPath::Ptr path = QmlBindingPath::create();

    QmlBindingPath::FileNode* fnode = new QmlBindingPath::FileNode;
    fnode->filePath = document->file()->path();

    n->parent = fnode;
    fnode->child = n;

    path->updatePath(fnode);

    tr.range = range;
    tr.bindingPath = path;

    return tr;
}

/**
 * \brief DocumentQmlInfo destructor
 */
DocumentQmlInfo::~DocumentQmlInfo(){
}

/**
 * \class lv::DocumentQmlInfo::Import
 * \ingroup lveditqmljs
 * \brief Import data associated with a lv::DocumentQmlInfo
 */

/**
 * \property lv::DocumentQmlInfo::Import::NoVersion
 * \brief Value used to check wether a version is valid or not
 */

// DocumentQmlInfo::Import implementation
// ----------------------------------------

/**
 * \brief Import constructor
 *
 * Takes an \p importType, the \p path of the import, the \p as namespace in which to import for this
 * document and the two versions: \p vMajor, \p vMinor
 */
DocumentQmlInfo::Import::Import(
        DocumentQmlInfo::Import::Type importType,
        const QString &path,
        const QString& as,
        int vMajor,
        int vMinor,
        Document::Location location)
    : m_type(importType)
    , m_uri(path)
    , m_relativeUri(path)
    , m_as(as)
    , m_versionMajor(vMajor)
    , m_versionMinor(vMinor)
    , m_location(location)
{
}

DocumentQmlInfo::Import::Import(
        DocumentQmlInfo::Import::Type importType,
        const QString &uri,
        const QString &relativeUri,
        const QString &as,
        int vMajor,
        int vMinor,
        Document::Location location)
    : m_type(importType)
    , m_uri(uri)
    , m_relativeUri(relativeUri)
    , m_as(as)
    , m_versionMajor(vMajor)
    , m_versionMinor(vMinor)
    , m_location(location)
{
}

QString DocumentQmlInfo::Import::toString() const{
    if ( m_type == DocumentQmlInfo::Import::Library ){
        return "import " + m_relativeUri  +
                (m_versionMajor >= 0
                    ? " " + QString::number(m_versionMajor) + "." + QString::number(m_versionMinor)
                    : "");
    } else if ( m_type == DocumentQmlInfo::Import::Invalid || m_relativeUri.isEmpty() ){
        return "";
    } else {
        return "import \"" + m_relativeUri + "\"" +
                  (m_versionMajor >= 0
                      ? " " + QString::number(m_versionMajor) + "." + QString::number(m_versionMinor)
                      : "");
    }
}

QString DocumentQmlInfo::Import::versionString() const{
    return QString::number(m_versionMajor) + "." + QString::number(m_versionMinor);
}

void DocumentQmlInfo::Import::setLocation(const Document::Location &location){
    m_location = location;
}

const Document::Location &DocumentQmlInfo::Import::location() const{
    return m_location;
}

QString DocumentQmlInfo::Import::join(const QList<DocumentQmlInfo::Import> &imports){
    QString result;
    for( auto import : imports ){
        result += (result.isEmpty() ? "" : "\n") + import.toString();
    }
    return result;
}

bool DocumentQmlInfo::hasImport(const DocumentQmlInfo::Import &key) const{
    foreach( const DocumentQmlInfo::Import& imp, d_ptr->imports ){
        if ( imp == key )
            return true;
    }
    return false;
}

bool DocumentQmlInfo::hasImport(const QString &importUri) const{
    foreach( const DocumentQmlInfo::Import& imp, d_ptr->imports ){
        if ( imp.uri() == importUri )
            return true;
    }
    return false;
}

bool DocumentQmlInfo::hasImportAs(const QString &asKey) const{
    foreach( const DocumentQmlInfo::Import& imp, d_ptr->imports ){
        if ( imp.uri() == asKey )
            return true;
    }
    return false;
}

/// \brief Returns the total number of imports for this lv::ProjectDocument.
int DocumentQmlInfo::totalImports() const{
    return d_ptr->imports.size();
}

/// \brief Returns the ImportList associated with this object.
const DocumentQmlInfo::ImportList &DocumentQmlInfo::imports() const{
    return d_ptr->imports;
}

void DocumentQmlInfo::transferImports(const DocumentQmlInfo::ImportList &imports){
    d_ptr->imports = imports;
}

/// \brief Adds an import \p path to a given \p key.
void DocumentQmlInfo::addImport(const DocumentQmlInfo::Import &key){
    d_ptr->imports.append(key);
}

/// \brief Updates the import at the given uri type.
void DocumentQmlInfo::updateImportType(const QString &uri, DocumentQmlInfo::Import::Type type){
    for( DocumentQmlInfo::Import& imp: d_ptr->imports ){
        if ( imp.uri() == uri )
            imp.setImportType(type);
    }
}

}// namespace

