#include "qdocumentqmlparser.h"
#include "qmljs/qmljsdocument.h"
#include "qmljs/qmljsbind.h"

namespace lcv{

QDocumentQmlParser::QDocumentQmlParser(){
}

QDocumentQmlParser::~QDocumentQmlParser()
{

}

QDocumentQmlInfo::MutablePtr QDocumentQmlParser::operator()(const QString &filePath, const QString &text){
    QDocumentQmlInfo::MutablePtr doc = QDocumentQmlInfo::create(filePath);
    doc->parse(text);
    return doc;
}

}// namespace
