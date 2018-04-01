/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#ifndef LVPROJECTQMLEXTENSION_H
#define LVPROJECTQMLEXTENSION_H

#include "live/lveditqmljsglobal.h"
#include "live/projectextension.h"

#include <QObject>

namespace lv{

class QmlJsSettings;
class ProjectQmlScope;
class CodeQmlHandler;
class ProjectQmlScanner;
class ProjectQmlScanMonitor;
class PluginInfoExtractor;
class LV_EDITQMLJS_EXPORT ProjectQmlExtension : public ProjectExtension{

public:
    ProjectQmlExtension(Settings* settings, Project* project, Engine *engine);
    virtual ~ProjectQmlExtension();

    AbstractCodeHandler* createHandler(
        ProjectDocument* document,
        Project* project,
        Engine* engine,
        QObject* parent
    );

    ProjectQmlScanMonitor* scanMonitor();
    ProjectQmlScanner* scanner();

    PluginInfoExtractor *getPluginInfoExtractor(const QString& import);

    static void engineHook(const QString& code, const QUrl& file, QObject* result, QObject* project);

private:
    Q_DISABLE_COPY(ProjectQmlExtension)

    QmlJsSettings*         m_settings;
    ProjectQmlScanMonitor* m_scanMonitor;
};

inline ProjectQmlScanMonitor *ProjectQmlExtension::scanMonitor(){
    return m_scanMonitor;
}

}// namespace

#endif // LVPROJECTQMLEXTENSION_H
