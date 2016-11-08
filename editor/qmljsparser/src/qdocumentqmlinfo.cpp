#include "qdocumentqmlinfo.h"
#include "qmljs/qmljsdocument.h"
#include "qmljs/qmljsbind.h"
#include "qqmlidvisitor.h"
#include "qdocumentqmlranges_p.h"

#include <QDebug>

namespace lcv{

class QDocumentQmlInfoPrivate{
public:
    QmlJS::Document::MutablePtr internalDoc;
    QmlJS::Bind*                internalDocBind;
    QDocumentQmlRanges          ranges;
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

QStringList QDocumentQmlInfo::extractIds(){
    Q_D(QDocumentQmlInfo);
    QIdExtractor extractor;

    d->internalDocBind->idEnvironment()->processMembers(&extractor);

    return extractor.ids();
}

const QDocumentQmlInfo::ValueReference QDocumentQmlInfo::rootObject(){
    Q_D(QDocumentQmlInfo);
    return QDocumentQmlInfo::ValueReference(d->internalDocBind->rootObjectValue(), this);
}

const QDocumentQmlInfo::ValueReference QDocumentQmlInfo::valueForId(const QString &id){
    Q_D(QDocumentQmlInfo);
    QIdValueExtractor valueExtractor(id);

    d->internalDocBind->idEnvironment()->processMembers(&valueExtractor);

    return QDocumentQmlInfo::ValueReference(valueExtractor.value(), this);
}

const QDocumentQmlObject QDocumentQmlInfo::extractValueObject(
        const ValueReference &valueref,
        ValueReference *parent)
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

//    qDebug() << DescribeValueVisitor::describe(valueref.value, 3);

    return vodata;
}

void QDocumentQmlInfo::createRanges(QTextDocument *document){
    Q_D(QDocumentQmlInfo);
    d->ranges(document, d->internalDoc);
}

const QDocumentQmlInfo::ValueReference QDocumentQmlInfo::valueAtCursor(const QTextCursor &){
    //TODO
    //Use bind->findQmlObject(AST::Node*)
    return QDocumentQmlInfo::ValueReference();
}

bool QDocumentQmlInfo::isValueNull(const QDocumentQmlInfo::ValueReference& vr){
    return vr.value == 0;
}

bool QDocumentQmlInfo::parse(const QString &source){
    Q_D(QDocumentQmlInfo);
    d->internalDoc->setSource(source);
    bool parseResult = d->internalDoc->parse();
    d->internalDocBind = d->internalDoc->bind();
    return parseResult;
}

QDocumentQmlInfo::~QDocumentQmlInfo(){
}

}// namespace

