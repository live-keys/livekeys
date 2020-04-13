#include "qmlwatcherbackground.h"

#include <QDir>
#include <QFileInfo>
#include <QDirIterator>

#include "live/documentqmlvalueobjects.h"
#include "live/documentqmlinfo.h"
#include "live/visuallogqt.h"

namespace lv{

QmlWatcherBackground::QmlWatcherBackground(
        const QString &filePath,
        int lineNumber,
        const QString &source,
        QObject *parent)
    : QThread(parent)
    , m_filePath(filePath)
    , m_lineNumber(lineNumber)
    , m_source(source)
{
}

QmlWatcherBackground::~QmlWatcherBackground(){
}

void QmlWatcherBackground::run(){
    DocumentQmlInfo::Ptr docinfo = DocumentQmlInfo::create(m_filePath);

    if ( !docinfo->parse(m_source) )
        return;

    docinfo->createRanges();

    // find offset
    int offset = 0;
    int currentLine = 1;
    while ( offset != -1 && currentLine < m_lineNumber){
        ++currentLine;
        offset = m_source.indexOf('\n', offset + 1);
    }

    if( offset == -1 )
        return;

    DocumentQmlInfo::ValueReference vr = docinfo->valueAtPosition(offset);
    if ( !docinfo->isValueNull(vr) ){
        DocumentQmlInfo::ValueReference vrparent;

        QmlTypeInfo::Ptr qti = docinfo->extractValueObject(vr, &vrparent);
        if ( !docinfo->isValueNull(vrparent) ){

            QStringList parentTypeName = docinfo->extractTypeName(vrparent);
            QString parentImportAs = parentTypeName.size() > 1 ? parentTypeName.first() : "";
            QString parentName     = parentTypeName.size() > 1 ? parentTypeName.last() : parentTypeName.first();

            QString parentFileName = parentName + ".qml";

            if ( parentImportAs.isEmpty() ){
                if ( existsInPath(QFileInfo(m_filePath).path(), parentFileName)){
                    m_result = QFileInfo(m_filePath).path() + "/" + parentFileName;
                    return;
                }
            }

            for ( const DocumentQmlInfo::Import& imp : docinfo->imports()){
                if ( imp.importType() == DocumentQmlInfo::Import::Directory ){
                    if ( imp.as() == parentImportAs ){
                        if ( existsInPath(imp.uri(), parentFileName) ){
                            m_result = imp.uri() + "/" + parentFileName;
                            return;
                        }
                    }
                }
            }
        }
    }
}

bool QmlWatcherBackground::existsInPath(const QString &path, const QString &name){
    return QFileInfo(path + "/" + name).exists();
}


}// namespace

