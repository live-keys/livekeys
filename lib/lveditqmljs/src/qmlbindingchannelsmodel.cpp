#include "qmlbindingchannelsmodel_p.h"
#include "documentqmlchannels.h"

namespace lv{

QmlBindingChannelsModel::QmlBindingChannelsModel(const QList<QmlBindingChannel::Ptr>& channels, QObject *parent)
    : QAbstractListModel(parent)
    , m_selectedIndex(-1)
    , m_channels(channels)
{
    m_roles[DocumentQmlChannels::Path]         = "path";
    m_roles[DocumentQmlChannels::IsConnected]  = "isConnected";
    m_roles[DocumentQmlChannels::RunnableName] = "runnableName";
    m_roles[DocumentQmlChannels::IsInSync]     = "isInSync";

    m_selectedIndex = m_channels.size() ? 0 : -1;
}

QmlBindingChannelsModel::~QmlBindingChannelsModel(){
}

int QmlBindingChannelsModel::rowCount(const QModelIndex &) const{
    return totalChannels();
}

QVariant QmlBindingChannelsModel::data(const QModelIndex &index, int role) const{
    if ( index.row() < m_channels.size() ){
        if ( role == QmlBindingChannelsModel::Path ){
            QStringList cp = DocumentQmlChannels::describePath(m_channels.at(index.row())->bindingPath());
            return cp;
        } else if ( role == QmlBindingChannelsModel::IsConnected ){
            return m_channels.at(index.row())->isEnabled();
        } else if ( role == QmlBindingChannelsModel::RunnableName ){
            QString path = m_channels.at(index.row())->runnable()->path();
            return path.mid(path.lastIndexOf('/') + 1);
        } else if ( role == DocumentQmlChannels::IsInSync ){
            QmlBindingChannel::Ptr ch = m_channels.at(index.row());
            if ( !ch->object() )
                return true;
            QVariant v = ch->object()->property("__connectionSync");
            if ( !v.isValid() )
                return true;
            return v.toBool();
        }
    }

    return QVariant();
}

const QmlBindingChannel::Ptr &QmlBindingChannelsModel::channelAt(int index) const{
    return m_channels.at(index);
}

void QmlBindingChannelsModel::selectChannel(int index){
    if ( index < m_channels.size() ){
        m_selectedIndex = index;
        emit selectedIndexChanged();
    }
}

}// namespace
