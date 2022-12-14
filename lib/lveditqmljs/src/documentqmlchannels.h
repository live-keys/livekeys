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

#ifndef LVDOCUMENTQMLCHANNELS_H
#define LVDOCUMENTQMLCHANNELS_H

#include <QObject>
#include <QList>
#include <QAbstractListModel>

#include "qmlbindingchannel.h"

namespace lv{

class LanguageQmlHandler;
class QmlBindingChannelsDispatcher;

/// \private
class DocumentQmlChannels : public QAbstractListModel{

    Q_OBJECT
    Q_PROPERTY(int selectedIndex READ selectedIndex NOTIFY selectedIndexChanged)

public:
    enum Roles{
        Path = Qt::UserRole + 1,
        IsConnected,
        RunnableName,
        IsInSync
    };

public:
    explicit DocumentQmlChannels(QmlBindingChannelsDispatcher* channelDispatcher, LanguageQmlHandler* handler);
    ~DocumentQmlChannels() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int selectedIndex() const;
    QmlBindingChannel::Ptr selectedChannel() const;

    void addChannel(QmlBindingChannel::Ptr bc);
    QmlBindingChannel::Ptr channelAt(int index) const;

    void updateChannelForRunnable(Runnable* r);
    void updateChannel(QmlBindingChannel::Ptr newChannel);

    int totalChannels() const;

    void removeChannels();
    void removeIndirectChannels();

    static QmlBindingChannel::Ptr traverseBindingPath(QmlBindingPath::Ptr path, Runnable* r);
    static QmlBindingChannel::Ptr traverseBindingPathFrom(QmlBindingChannel::Ptr from, QmlBindingPath::Ptr path);

    static QStringList describePath(const QmlBindingPath::Ptr& bp);

    void desyncInactiveChannels();

    bool canRebuild() const;
    void rebuild();

    void runnableBuildReady(Runnable* r);

public slots:
    lv::ProjectDocument* document() const;
    void selectChannel(int index);
    lv::Runnable* selectedChannelRunnable() const;

signals:
    void selectedIndexChanged();
    void selectedChannelChanged();

private:
    void initialize();

    int                    m_selectedIndex;
    LanguageQmlHandler*    m_languageHandler;
    QHash<int, QByteArray> m_roles;
    QmlBindingChannel::Ptr m_selectedChannel;

    QList<QmlBindingChannel::Ptr> m_channels;
    QmlBindingChannelsDispatcher* m_channelDispatcher;
};

inline QHash<int, QByteArray> DocumentQmlChannels::roleNames() const{
    return m_roles;
}

inline int DocumentQmlChannels::selectedIndex() const{
    return m_selectedIndex;
}

inline int DocumentQmlChannels::totalChannels() const{
    return m_channels.size();
}

}// namespace

#endif // LVDOCUMENTQMLCHANNELS_H
