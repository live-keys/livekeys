#include "qmlprojectinfo.h"
#include "live/viewengine.h"
#include <QCoreApplication>

namespace lv{

QmlProjectInfo::QmlProjectInfo(const QString &path, const QUrl &programPath, ViewEngine *viewEngine, QObject *parent)
    : QObject(parent)
    , m_path(path)
    , m_programPath(programPath)
    , m_viewEngine(viewEngine)
{
}

QmlProjectInfo::~QmlProjectInfo(){
}

void QmlProjectInfo::setArguments(const QStringList &arguments){
    m_arguments = arguments;
}

const QStringList &QmlProjectInfo::arguments() const{
    return m_arguments;
}

QJSValue QmlProjectInfo::scriptArgv() const{
    QJSValue result = m_viewEngine->engine()->newArray(m_arguments.length());
    for ( int i = 0; i < m_arguments.length(); ++i ){
        result.setProperty(i, m_arguments[i]);
    }
    return result;
}

void QmlProjectInfo::onExit(const std::function<void (int)> &exitHandler){
    m_exitHandler = exitHandler;
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

void QmlProjectInfo::exit(int code){
    if ( m_exitHandler ){
        m_exitHandler(code);
    } else {
        m_viewEngine->engine()->exit(code);
    }
}

}// namespace
