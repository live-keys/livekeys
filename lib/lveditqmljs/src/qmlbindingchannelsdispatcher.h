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

#ifndef LVQMLBINDINGCHANNELSDISPATCHER_H
#define LVQMLBINDINGCHANNELSDISPATCHER_H

#include <QObject>
#include "live/project.h"

namespace lv{

class LanguageQmlHandler;
class DocumentQmlChannels;

/// \private
class QmlBindingChannelsDispatcher : public QObject{

    Q_OBJECT

public:
    explicit QmlBindingChannelsDispatcher(Project* project, QObject *parent = nullptr);
    ~QmlBindingChannelsDispatcher() override;

    void initialize(LanguageQmlHandler* codeHandler, DocumentQmlChannels* documentChannels);
    void removeDocumentChannels(DocumentQmlChannels* documentChannels);

signals:

public slots:
    void __newQmlBuild(Runnable* runnable, QmlBuild* build);
    void __qmlBuildReady();
    void __hookEntryAdded(Runnable* runnable, const QString& file, const QString& id, QObject* object);


private:
    Project*                   m_project;
    QSet<DocumentQmlChannels*> m_channeledDocuments;

};

}// namespace

#endif // LVQMLBINDINGCHANNELSDISPATCHER_H
