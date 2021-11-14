#include "projectengineinterceptor.h"
#include "live/viewengine.h"
#include "live/project.h"

namespace lv{

ProjectEngineInterceptor::ProjectEngineInterceptor(ViewEngine *, Project *project)
    : ViewEngineInterceptor()
    , m_project(project)
{
}

ViewEngineInterceptor::ContentResult ProjectEngineInterceptor::interceptContent(const QUrl &url){
    if (url.scheme() == "memory"){
        QUrl urlCopy = url;
        urlCopy.setScheme("file");
        return ViewEngineInterceptor::ContentResult(
            true, m_project->openFile(urlCopy.toLocalFile(), "binary", Document::EditIfNotOpen)->content()
        );
    }
    return ViewEngineInterceptor::ContentResult(false);
}

QUrl ProjectEngineInterceptor::interceptUrl(const QUrl &path, QQmlAbstractUrlInterceptor::DataType dataType){
    if (dataType == QQmlAbstractUrlInterceptor::QmlFile && path.isLocalFile()){
        if (m_project->isOpened(path.toLocalFile())){
            QUrl memoryPath(path);
            memoryPath.setScheme("memory");
            return memoryPath;
        }
    }
    return path;
}

}// namespace
