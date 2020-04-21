#ifndef LVTRACKLISTMODEL_H
#define LVTRACKLISTMODEL_H

#include <QAbstractListModel>
#include "qabstractrangemodel.h"

namespace lv{

class Track;

/// \private
class TrackListModel : public QAbstractListModel{

    Q_OBJECT

public:
    enum Roles{
        TrackRole = Qt::UserRole + 1
    };

public:
    explicit TrackListModel(QObject *parent = nullptr);
    ~TrackListModel();

    virtual QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role) const;
    int  rowCount(const QModelIndex &) const;

    void removeTrack(Track* track);

signals:

public slots:
    void appendTrack(Track *track);
    Track *appendTrack();
    void removeTrack(int index);
    lv::Track* trackAt(int index) const;
    void clearTracks();

    int totalTracks() const;

private:
    QHash<int, QByteArray>      m_roles;
    QList<Track*>               m_tracks;

};

inline QHash<int, QByteArray> TrackListModel::roleNames() const{
    return m_roles;
}

inline int TrackListModel::rowCount(const QModelIndex&) const{
    return totalTracks();
}

}// namespace

#endif // LVTRACKLISTMODEL_H
