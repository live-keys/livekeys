#include "qdocumentqmlinfo.h"
#include "qmljs/qmljsdocument.h"
#include "qmljs/qmljsbind.h"
#include "qqmlidvisitor.h"

#include <QDebug>

namespace lcv{

class QDocumentQmlInfoPrivate{
public:
    QmlJS::Document::MutablePtr internalDoc;
    QmlJS::Bind*                internalDocBind;
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

const QmlJS::Value *QDocumentQmlInfo::valueForId(const QString &id){
    Q_D(QDocumentQmlInfo);
    QIdValueExtractor valueExtractor(id);

    d->internalDocBind->idEnvironment()->processMembers(&valueExtractor);

    return valueExtractor.value();
}

const QDocumentQmlInfo::ValueObject QDocumentQmlInfo::extractValueData(
        const QmlJS::Value* value,
        const QmlJS::Value** parent)
{
    Q_D(QDocumentQmlInfo);
    QDocumentQmlInfo::ValueObject vodata;
    if ( const QmlJS::ASTObjectValue* vob = value->asAstObjectValue()){
        vodata.className = vob->className();
        QValueMemberExtractor extractor;
        vob->processMembers(&extractor);
    }

//    qDebug() << DescribeValueVisitor::describe(value, 3);

    return vodata;
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

