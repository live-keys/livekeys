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
#include "live/documenthandler.h"
#include "live/abstractcodehandler.h"
#include "live/palettecontainer.h"

#include <QObject>
#include <QQmlParserStatus>

namespace lv{

class Settings;
class QmlJsSettings;
class ProjectQmlScope;
class CodeQmlHandler;
class ProjectQmlScanner;
class ProjectQmlScanMonitor;
class PluginInfoExtractor;
class LV_EDITQMLJS_EXPORT ProjectQmlExtension : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

public:
    ProjectQmlExtension(QObject* parent = 0);
    virtual ~ProjectQmlExtension();

    void classBegin();
    void componentComplete();

    ProjectQmlScanMonitor* scanMonitor();
    ProjectQmlScanner* scanner();

    PluginInfoExtractor *getPluginInfoExtractor(const QString& import);

    static void engineHook(const QString& code, const QUrl& file, QObject* result, QObject* project, void *data);

    void addCodeQmlHandler(CodeQmlHandler* handler);
    void removeCodeQmlHandler(CodeQmlHandler* handler);

    PaletteContainer* paletteContainer();

    static void registerTypes(const char* uri);

    void setParams(Settings *settings, Project* project, ViewEngine* engine);

public slots:
    QObject* createHandler(ProjectDocument* document, DocumentHandler* handler);

private:
    Q_DISABLE_COPY(ProjectQmlExtension)

    Project*               m_project;
    ViewEngine*            m_engine;
    QmlJsSettings*         m_settings;
    ProjectQmlScanMonitor* m_scanMonitor;
    QList<CodeQmlHandler*> m_codeHandlers;
    PaletteContainer*  m_paletteContainer;
};

/// \brief Returns the lv::ProjectQmlScanMonitor associated with this object.
inline ProjectQmlScanMonitor *ProjectQmlExtension::scanMonitor(){
    return m_scanMonitor;
}

/// \brief Returns the lv::PaletteContainer associated with this object.
inline PaletteContainer *ProjectQmlExtension::paletteContainer(){
    return m_paletteContainer;
}

}// namespace

#endif // LVPROJECTQMLEXTENSION_H
