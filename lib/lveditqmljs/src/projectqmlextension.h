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

#ifndef LVPROJECTQMLEXTENSION_H
#define LVPROJECTQMLEXTENSION_H

#include "live/lveditqmljsglobal.h"
#include "live/codehandler.h"
#include "live/paletteloader.h"
#include "live/qmllanguageinfo.h"

#include <QObject>
#include <QQmlParserStatus>

namespace lv{

class Settings;
class Workspace;
class QmlJsSettings;
class ProjectQmlScope;
class LanguageQmlHandler;
class ProjectQmlScanner;
class QmlProjectMonitor;
class QmlLanguageScanner;
class QmlBindingChannelsDispatcher;

class LV_EDITQMLJS_EXPORT ProjectQmlExtension : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

public:
    ProjectQmlExtension(QObject* parent = nullptr);
    virtual ~ProjectQmlExtension();

    void classBegin();
    void componentComplete();

    QmlProjectMonitor* scanMonitor();
    QmlBindingChannelsDispatcher* channelsDispatcher();

    void addLanguageQmlHandler(LanguageQmlHandler* handler);
    void removeLanguageQmlHandler(LanguageQmlHandler* handler);

    PaletteLoader* paletteContainer();

    static void registerTypes(const char* uri);

    Project* project();

    static bool pluginTypesEnabled();
    static QmlLibraryInfo::ScanStatus loadPluginInfoInternal(
        QmlLanguageScanner *scanner,
        QmlLibraryInfo::Ptr lib,
        QQmlEngine *engine,
        QByteArray *stream);

public slots:
    QObject* createHandler(ProjectDocument* document, CodeHandler* handler);

private:
    Q_DISABLE_COPY(ProjectQmlExtension)

    Project*                      m_project;
    ViewEngine*                   m_engine;
    QmlJsSettings*                m_settings;
    QmlProjectMonitor*            m_scanMonitor;
    QList<LanguageQmlHandler*>    m_languageHandlers;
    PaletteLoader*                m_paletteContainer;
    QmlBindingChannelsDispatcher* m_channelDispatcher;
};

/// \brief Returns the lv::ProjectQmlScanMonitor associated with this object.
inline QmlProjectMonitor *ProjectQmlExtension::scanMonitor(){
    return m_scanMonitor;
}

/// \brief Returns the global lv::Project
inline Project *ProjectQmlExtension::project(){
    return m_project;
}

/// \brief Returns the channels dispatcher
inline QmlBindingChannelsDispatcher *ProjectQmlExtension::channelsDispatcher(){
    return m_channelDispatcher;
}


}// namespace

#endif // LVPROJECTQMLEXTENSION_H
