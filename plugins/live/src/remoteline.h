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

#ifndef LVREMOTELINE_H
#define LVREMOTELINE_H

#include <QObject>
#include <QQmlPropertyMap>
#include <QSet>

#include "componentsource.h"
#include "remotecontainer.h"

namespace lv{

class RemoteLineProperty;

/// \private
class RemoteLine : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(lv::ComponentSource* source     READ source     WRITE setSource     NOTIFY sourceChanged)
    Q_PROPERTY(lv::RemoteContainer* connection READ connection WRITE setConnection NOTIFY connectionChanged)
    Q_PROPERTY(QQmlPropertyMap* result         READ result     NOTIFY resultChanged)
    Q_CLASSINFO("DefaultProperty", "source")

    friend class RemoteLineProperty;

public:
    explicit RemoteLine(QObject *parent = nullptr);
    ~RemoteLine();

    void classBegin() Q_DECL_OVERRIDE{}
    void componentComplete() Q_DECL_OVERRIDE;

    bool isComponentComplete() const;

    lv::ComponentSource* source() const;
    void setSource(lv::ComponentSource* source);

    RemoteContainer *connection() const;
    void setConnection(lv::RemoteContainer* connection);

    static void receiveMessage(const LineMessage& message, void* data);
    void onMessage(const LineMessage& message);

    QQmlPropertyMap* result() const;

public slots:
    void initialize();

signals:
    void complete();
    void sourceChanged();
    void connectionChanged();
    void resultChanged();

private:
    void propertyChanged(RemoteLineProperty* property);
    void sendProperty(const QString& propertyName);

    QList<RemoteLineProperty*> m_properties;
    bool                    m_componentComplete;
    bool                    m_componentBuild;
    lv::ComponentSource*    m_source;
    lv::RemoteContainer*    m_connection;
    QQmlPropertyMap*        m_result;

    QSet<QString>           m_propertiesToSend;
};

inline bool RemoteLine::isComponentComplete() const{
    return m_componentComplete;
}

inline ComponentSource *RemoteLine::source() const{
    return m_source;
}

inline RemoteContainer *RemoteLine::connection() const{
    return m_connection;
}

inline QQmlPropertyMap *RemoteLine::result() const{
    return m_result;
}

}// namespace

#endif // LVREMOTELINE_H
