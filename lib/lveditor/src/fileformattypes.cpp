/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

QString FileFormatTypes::fileFilterFromFormat(const QString &format){
    if ( format.isEmpty() )
        return "";

    for ( FileFormatTypes::Entry& e : m_entries ){
        if ( e.type == format ){
            if ( !e.matchExtension.isEmpty() ){
                return
                    e.matchExtension.at(0).toUpper() + e.matchExtension.mid(1) +
                    " files (*." + e.matchExtension + ")";
            } else if ( !e.matchWildCard.isEmpty() ){
                return
                    format.at(0).toUpper() + format.mid(1) +
                    " files (" + e.matchWildCard + ")";
            } else {
                return "";
            }
        }
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