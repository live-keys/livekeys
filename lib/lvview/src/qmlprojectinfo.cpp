#include "qmlprojectinfo.h"

namespace lv{

QmlProjectInfo::QmlProjectInfo(const QString &path, const QUrl &programPath, QObject *parent)
    : QObject(parent)
    , m_path(path)
    , m_programPath(programPath)
{
}

QmlProjectInfo::~QmlProjectInfo(){
}

QUrl QmlProjectInfo::program() const{
    return m_programPath;
}

QString QmlProjectInfo::dir() const{
    return m_path;
}

QString QmlProjectInfo::path(const QString &relative) const{
    return dir() + "/" + relative;
}

}// namespace
