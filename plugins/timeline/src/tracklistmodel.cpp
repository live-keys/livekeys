#include "tracklistmodel.h"
#include "track.h"
#include "qganttmodel.h"

#include "live/visuallogqt.h"
#include "live/mlnode.h"
#include "live/viewengine.h"

#include <QQmlEngine>
#include <QJSValue>

namespace lv{

TrackListModel::TrackListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roles[TrackRole] = "track";
}

TrackListModel::~TrackListModel(){
    clearTracks();
}

QVariant TrackListModel::data(const QModelIndex& index, int role) const{
    if ( role == TrackListModel::TrackRole)
        return QVariant::fromValue(m_tracks[index.row()]);
    return QVariant();
}

void TrackListModel::clearTracks(){
    beginResetModel();
    for ( Track* track : m_tracks ){
        track->deleteLater();
        track->setParent(nullptr);
    }
    m_tracks.clear();
    endResetModel();
}

Track* TrackListModel::appendTrack(){
    Track* track = new Track(this);
    track->setName("Track #" + QString::number(m_tracks.length() + 1));
    appendTrack(track);
    return track;
}

lv::Track *TrackListModel::trackAt(int index) const{
    return m_tracks.at(index);
}

int TrackListModel::totalTracks() const{
    return m_tracks.size();
}

void TrackListModel::removeTrack(Track *track){
    if ( !track )
        return;
    int index = m_tracks.indexOf(track);
    if ( index == -1 )
        return;
    removeTrack(index);
}

void TrackListModel::serialize(ViewEngine *engine, const QObject *o, MLNode &node){
    const TrackListModel* trackList = qobject_cast<const TrackListModel*>(o);

    node = MLNode(MLNode::Array);

    for ( int i = 0; i < trackList->m_tracks.size(); ++i ){
        MLNode trackNode;
        Track::serialize(engine, trackList->trackAt(i), trackNode);
        node.append(trackNode);
    }
}

QObject *TrackListModel::deserialize(ViewEngine *, const MLNode &){
    return nullptr;
}

void TrackListModel::removeTrack(int index){
    auto it = m_tracks.begin() + index;
    emit trackAboutToBeRemoved(*it);

    beginRemoveRows(QModelIndex(), index, index);
    m_tracks.erase(it);
    delete *it;
    endRemoveRows();
}

void TrackListModel::appendTrack(Track *track){
    beginInsertRows(QModelIndex(), m_tracks.length(), m_tracks.length());
    m_tracks.append(track);
    endInsertRows();
    emit trackAdded(track);
}

}// namespace
