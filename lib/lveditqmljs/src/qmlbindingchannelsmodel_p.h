#ifndef LVQMLBINDINGCHANNELSMODEL_H
#define LVQMLBINDINGCHANNELSMODEL_H

#include <QObject>
#include <QAbstractListModel>

#include "qmlbindingchannel.h"

namespace lv{

class QmlBindingChannelsModel : public QAbstractListModel{

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
    explicit QmlBindingChannelsModel(const QList<QmlBindingChannel::Ptr>& channels, QObject *parent = nullptr);
    ~QmlBindingChannelsModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int selectedIndex() const;

    const QmlBindingChannel::Ptr& channelAt(int index) const;

public slots:
    int totalChannels() const;
    void selectChannel(int index);

signals:
    void selectedIndexChanged();

private:
    int                           m_selectedIndex;
    QHash<int, QByteArray>        m_roles;
    QList<QmlBindingChannel::Ptr> m_channels;
};

inline QHash<int, QByteArray> QmlBindingChannelsModel::roleNames() const{
    return m_roles;
}
inline int QmlBindingChannelsModel::selectedIndex() const{
    return m_selectedIndex;
}

inline int QmlBindingChannelsModel::totalChannels() const{
    return m_channels.size();
}

}// namespace

#endif // LVQMLBINDINGCHANNELSMODEL_H
