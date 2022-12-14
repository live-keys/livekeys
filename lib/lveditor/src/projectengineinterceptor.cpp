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

#include "projectengineinterceptor.h"
#include "live/viewengine.h"
#include "live/project.h"

namespace lv{

ProjectEngineInterceptor::ProjectEngineInterceptor(ViewEngine *engine, Project *project)
    : ViewEngineInterceptor()
    , m_engine(engine)
    , m_project(project)
{
}

ViewEngineInterceptor::ContentResult ProjectEngineInterceptor::interceptContent(const QUrl &url){
    if (url.scheme() == "memory"){
        QUrl urlCopy = url;
        urlCopy.setScheme("file");
        Document* doc = m_project->isOpened(urlCopy.toLocalFile());
        if ( doc ){
            return ViewEngineInterceptor::ContentResult(true, doc->content());
        } else {
            std::string s = m_engine->fileIO()->readFromFile(urlCopy.toLocalFile().toStdString());
            return ViewEngineInterceptor::ContentResult(true, QByteArray::fromStdString(s));
        }
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
