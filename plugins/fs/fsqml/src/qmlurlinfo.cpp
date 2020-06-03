#include "qmlurlinfo.h"

namespace lv{

QmlUrlInfo::QmlUrlInfo(QObject *parent)
    : QObject(parent)
{
}

QUrl QmlUrlInfo::urlFromLocalFile(const QString &filePath, const QJSValue &opt){
    QUrl url = QUrl::fromLocalFile(filePath);
    if ( opt.hasProperty("fragment") )
        url.setFragment(opt.property("fragment").toString());
    if ( opt.hasProperty("query") )
        url.setQuery(opt.property("query").toString());
    return url;
}

}// namespace
