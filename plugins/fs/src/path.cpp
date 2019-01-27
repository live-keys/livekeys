#include "path.h"
#include <QFileInfo>

namespace lv{

Path::Path(QObject *parent)
    : QObject(parent)
{
}

QString Path::name(const QString &path){
    return QFileInfo(path).fileName();
}

}// namespace
