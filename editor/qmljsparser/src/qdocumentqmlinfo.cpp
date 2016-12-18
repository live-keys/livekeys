#include "qdocumentqmlinfo.h"
#include "qmljs/qmljsdocument.h"
#include "qmljs/qmljsbind.h"
#include "qqmlidvisitor_p.h"
#include "qdocumentqmlranges_p.h"

#include <QDebug>

namespace lcv{

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
    else if ( dialect == QDocumentQmlInfo::Qml ){
        d->internalDoc = QmlJS::Document::create(fileName, QmlJS::Dialect::Qml);
    } else
        d->internalDoc = QmlJS::Document::create(fileName, QmlJS::Dialect::NoLanguage);
}

QDocumentQmlInfo::MutablePtr QDocumentQmlInfo::create(const QString &fileName){
    return QDocumentQmlInfo::MutablePtr(new QDocumentQmlInfo(fileName));
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
        QDocumentQmlInfo::Message::Severity severity;
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

QDocumentQmlInfo::~QDocumentQmlInfo(){
}

}// namespace

