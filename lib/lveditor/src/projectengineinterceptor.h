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

#ifndef LVPROJECTENGINEINTERCEPTOR_H
#define LVPROJECTENGINEINTERCEPTOR_H

#include <QObject>
#include "live/viewengineinterceptor.h"

namespace lv{

class Project;
class ViewEngine;
class ProjectEngineInterceptor : public ViewEngineInterceptor{

public:
    explicit ProjectEngineInterceptor(ViewEngine* engine, Project* project);

public:
    QUrl interceptUrl(const QUrl &path, QQmlAbstractUrlInterceptor::DataType dataType) override;
    ContentResult interceptContent(const QUrl& path) override;

private:
    Project*    m_project;
};

}// namespace

#endif // LVPROJECTENGINEINTERCEPTOR_H
