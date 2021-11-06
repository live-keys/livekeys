#include "fileformattypes.h"
#include "live/visuallogqt.h"
#include <QRegularExpression>
#include <QFileInfo>
#include <QJSValueIterator>

namespace lv{

FileFormatTypes::FileFormatTypes(QObject *parent)
    : QObject(parent)
{
}

FileFormatTypes::~FileFormatTypes(){
}

void FileFormatTypes::add(const QJSValue &entries){
    QJSValueIterator vit(entries);
    while ( vit.hasNext() ){
        vit.next();
        if ( vit.name() != "length" ){
            QJSValue opt = vit.value();
            if ( opt.hasOwnProperty("type") ){

                QString type  = opt.property("type").toString();
                QString name  = opt.hasOwnProperty("name") ? opt.property("name").toString() : "";
                QString ext   = opt.hasOwnProperty("extension") ? opt.property("extension").toString() : "";
                QString match = opt.hasOwnProperty("match") ? opt.property("match").toString() : "";

                addEntry(type, name, ext, match);
            }
        }
    }
}

void FileFormatTypes::addEntry(const QString &type, const QString &name, const QString &extension, const QString &match){
    FileFormatTypes::Entry e;
    e.type = type;
    e.matchName = name;
    e.matchExtension = extension;
    e.matchWildCard = match;
    m_entries.append(e);
}

QString FileFormatTypes::find(const QString &path){
    QFileInfo fi(path);
    QString name = fi.fileName();

    for ( FileFormatTypes::Entry& e : m_entries ){
        if ( e.isMatch(name) )
            return e.type;
    }

    return "";
}

bool FileFormatTypes::Entry::isMatch(const QString &file){
    if ( !matchName.isEmpty() ){
        if ( matchName != file )
            return false;
    }
    if ( !matchExtension.isEmpty() ){
        int pos = file.lastIndexOf('.');
        QString ext = pos == -1 ? "" : file.mid(pos + 1);
        if ( ext != matchExtension )
            return false;
    }
    if ( !matchWildCard.isEmpty() ){
        QString wildcard = QRegularExpression::wildcardToRegularExpression(matchWildCard);
        QRegularExpression re(wildcard);
        QRegularExpressionMatch match = re.match(file);
        if ( !match.hasMatch() )
            return false;
    }
    return true;
}

}// namespace
