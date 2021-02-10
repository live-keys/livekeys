#include "documentqmlchannels.h"

#include "live/visuallogqt.h"
#include "live/project.h"
#include "live/projectdocument.h"
#include "live/runnablecontainer.h"
#include "live/codeqmlhandler.h"
#include "live/projectfile.h"
#include "live/hookcontainer.h"
#include "qmlwatcher.h"
#include "qmlbuilder.h"


#include "qmlbindingchannelsdispatcher.h"

namespace lv{

namespace{

/// \private
QmlBindingChannel::Ptr traversePath(QmlBindingPath::Ptr path, Runnable* r, QmlBindingPath::Node* n, QObject* object){
    if ( n == nullptr || object == nullptr)
        return QmlBindingChannel::Ptr(nullptr);

    if ( n->type() == QmlBindingPath::Node::Property ){
        QmlBindingPath::PropertyNode* pn = static_cast<QmlBindingPath::PropertyNode*>(n);

        QQmlProperty prop(object, pn->propertyName);
        if ( !prop.isValid() ){
            int findex = pn->propertyName.indexOf('(');
            if ( findex == -1 )
                return QmlBindingChannel::Ptr(nullptr);

            QByteArray upname = pn->propertyName.toUtf8();
            int methodIndex = object->metaObject()->indexOfMethod(upname.data());
            if ( methodIndex >= 0 ){
                return QmlBindingChannel::create(path, r, QQmlProperty(object, "objectName"), object->metaObject()->method(methodIndex));
            }
        }

        if ( n->child == nullptr ){
            return QmlBindingChannel::create(path, r, prop);
        } else {
            return traversePath(path, r, n->child, prop.read().value<QObject*>());
        }
    } else if ( n->type() == QmlBindingPath::Node::Index ){
        QmlBindingPath::IndexNode* in = static_cast<QmlBindingPath::IndexNode*>(n);

        QQmlProperty prop(object);
        if ( !prop.isValid() )
            return QmlBindingChannel::Ptr(nullptr);
        if ( prop.propertyTypeCategory() == QQmlProperty::Object && in->index != 0 )
            return QmlBindingChannel::Ptr(nullptr);

        if ( n->child == nullptr ){
            return QmlBindingChannel::create(path, r, prop, in->index);
        } else {
            if ( prop.propertyTypeCategory() == QQmlProperty::Object ){
                return traversePath(path, r, n->child, prop.read().value<QObject*>());
            } else if ( prop.propertyTypeCategory() == QQmlProperty::List ){
                QQmlListReference ppref = qvariant_cast<QQmlListReference>(prop.read());
                if ( ppref.canAt() && ppref.canCount() && ppref.count() > in->index ){
                    QObject* parent = ppref.count() > 0 ? ppref.at(0)->parent() : ppref.object();
                    if ( !parent )
                        return QmlBindingChannel::Ptr(nullptr);

                    // create correct order for list reference
                    QObjectList ordered;
                    for (auto child: parent->children())
                    {
                        bool found = false;
                        for (int i = 0; i < ppref.count(); ++i)
                            if (child == ppref.at(i)){
                                found = true;
                                break;
                            }
                        if (found) ordered.push_back(child);
                    }

                    return traversePath(path, r, n->child, ordered[in->index]);
                } else
                    return QmlBindingChannel::Ptr(nullptr);
            }
        }
    } else if ( n->type() == QmlBindingPath::Node::File ){
        return traversePath(path, r, n->child, object);
    } else if ( n->type() == QmlBindingPath::Node::Component ){
        return traversePath(path, r, n->child, object);
    }
    return QmlBindingChannel::Ptr(nullptr);

}

}// namespace

DocumentQmlChannels::DocumentQmlChannels(QmlBindingChannelsDispatcher* channelDispatcher, CodeQmlHandler *handler)
    : QAbstractListModel(handler)
    , m_selectedIndex(-1)
    , m_codeHandler(handler)
    , m_channelDispatcher(channelDispatcher)
{
    m_roles[DocumentQmlChannels::Path]         = "path";
    m_roles[DocumentQmlChannels::IsConnected]  = "isConnected";
    m_roles[DocumentQmlChannels::RunnableName] = "runnableName";
    m_roles[DocumentQmlChannels::IsInSync]     = "isInSync";

    m_channelDispatcher->initialize(m_codeHandler, this);
}

DocumentQmlChannels::~DocumentQmlChannels(){
    m_channelDispatcher->removeDocumentChannels(this);
}

int DocumentQmlChannels::rowCount(const QModelIndex &) const{
    return totalChannels();
}

QVariant DocumentQmlChannels::data(const QModelIndex &index, int role) const{
    if ( index.row() < m_channels.size() ){
        if ( role == DocumentQmlChannels::Path ){
            QStringList cp = describePath(m_channels.at(index.row())->bindingPath());
            return cp;
        } else if ( role == DocumentQmlChannels::IsConnected ){
            return m_channels.at(index.row())->isEnabled();
        } else if ( role == DocumentQmlChannels::RunnableName ){
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

QmlBindingChannel::Ptr DocumentQmlChannels::selectedChannel() const{
    if ( selectedIndex() == -1 )
        return nullptr;
    return m_channels[m_selectedIndex];
}

void DocumentQmlChannels::addChannel(QmlBindingChannel::Ptr bc){
    beginInsertRows(QModelIndex(), m_channels.size(), m_channels.size());
    m_channels.append(bc);

    bool indexChanged = false;
    if ( m_selectedIndex == -1 ){
        m_selectedIndex = m_channels.size() - 1;
        m_selectedChannel = m_channels[m_selectedIndex];
        indexChanged = true;
    }

    endInsertRows();

    if ( indexChanged ){
        emit selectedChannelChanged();
        emit selectedIndexChanged();
    }
}

QmlBindingChannel::Ptr DocumentQmlChannels::channelAt(int index) const{
    return m_channels.at(index);
}

void DocumentQmlChannels::selectChannel(int index){
    if ( m_selectedIndex != index && index < m_channels.size() ){
        if ( m_selectedChannel ){
            m_selectedChannel->setEnabled(false);
        }
        m_selectedChannel = m_channels[index];
        m_selectedChannel->setEnabled(true);
        m_selectedIndex = index;

        emit selectedIndexChanged();
        emit selectedChannelChanged();
    }
}

void DocumentQmlChannels::updateChannelForRunnable(Runnable *r){
    QmlBindingPath::Ptr runnableBp = QmlBindingPath::create();
    runnableBp->appendFile(r->path());

    for ( int i = 0; i < m_channels.size(); ++i ){
        QmlBindingChannel::Ptr& bc = m_channels[i];
        if ( bc->type() == QmlBindingChannel::Object ){
            QmlBindingPath::Ptr bp = bc->bindingPath();
            if ( *bp == *runnableBp ){
                bc->updateConnection(r->viewRoot());
                if ( m_selectedIndex == i ){
                    emit selectedChannelChanged();
                }

                return;
            }
        }
    }

    QmlBindingChannel::Ptr bc = DocumentQmlChannels::traverseBindingPath(runnableBp, r);
    addChannel(bc);
}

void DocumentQmlChannels::updateChannel(QmlBindingChannel::Ptr newChannel){
    for ( int i = 0; i < m_channels.size(); ++i ){
        QmlBindingChannel::Ptr& bc = m_channels[i];
        QmlBindingPath::Ptr channelPath = bc->bindingPath();

        if ( *channelPath == *newChannel->bindingPath() ){
            bc->clearConnection();
            m_channels[i] = newChannel;

            if ( m_selectedIndex == i ){
                emit selectedChannelChanged();
            }

            return;
        }
    }

    addChannel(newChannel);
}

void DocumentQmlChannels::removeChannels(){
    m_channels.clear();
    m_selectedChannel = nullptr;
    m_selectedIndex = -1;
    emit selectedChannelChanged();
    emit selectedIndexChanged();
}

ProjectDocument *DocumentQmlChannels::document() const{
    return m_codeHandler->document();
}

/**
 * \brief Match the binding \p path with the application value and update it's connection
 *
 * This function traverses the binding path recursively starting from the \p root of
 * the application and tries to match the \p path with an application value.
 *
 * If the match is set, then the \p path value will be udpated.
 */
QmlBindingChannel::Ptr DocumentQmlChannels::traverseBindingPath(QmlBindingPath::Ptr path, Runnable *r){
    if ( !path->root() || !r || !r->viewRoot())
        return nullptr;

    QmlBindingPath::Node* root = path->root();

    QObject* viewRoot = r->viewRoot();

    while ( root && (root->type() == QmlBindingPath::Node::File || root->type() == QmlBindingPath::Node::Component) ){
        root = root->child;
    }

    if ( root && root->type() == QmlBindingPath::Node::Watcher ){
        QmlBindingPath::WatcherNode* wnode = static_cast<QmlBindingPath::WatcherNode*>(root);

        HookContainer* hooks = qobject_cast<HookContainer*>(
            r->viewContext()->contextProperty("hooks").value<QObject*>()
        );

        if ( !hooks )
            return nullptr;

        QList<QObject*> watchers = hooks->entriesFor(wnode->filePath, wnode->objectId);

        if ( watchers.isEmpty() )
            return nullptr;

        QmlWatcher* watcher = qobject_cast<QmlWatcher*>(watchers.first());
        viewRoot = watcher->target();

        root = root->child;
    }

    while ( root && (root->type() == QmlBindingPath::Node::File || root->type() == QmlBindingPath::Node::Component) ){
        root = root->child;
    }

    if ( !root ){
        if ( viewRoot ){
            return QmlBindingChannel::create(path, r, viewRoot);
        }
        return nullptr;
    }

    if ( root->child == nullptr ){
        QQmlProperty prop(viewRoot->parent());
        if( !prop.isValid() )
            return QmlBindingChannel::Ptr();
        return QmlBindingChannel::create(path, r, prop, 0);
    }

    return traversePath(path, r, root->child, viewRoot);
}

QmlBindingChannel::Ptr DocumentQmlChannels::traverseBindingPathFrom(QmlBindingChannel::Ptr from, QmlBindingPath::Ptr path){
    if ( from.isNull() )
        return nullptr;
    Runnable* r = from->runnable();
    if ( !path->root() || !r || !r->viewRoot())
        return nullptr;

    if ( from->isBuilder() ){
        QmlBindingChannel::Ptr builderChannel = QmlBindingChannel::create(path, r);
        builderChannel->setIsBuilder(true);
        return builderChannel;
    }

    QmlBindingPath::Node* root = path->root();

    QObject* viewRoot = from->object();

    if ( root && root->type() == QmlBindingPath::Node::Watcher ){
        QmlBindingPath::WatcherNode* wnode = static_cast<QmlBindingPath::WatcherNode*>(root);

        HookContainer* hooks = qobject_cast<HookContainer*>(
            r->viewContext()->contextProperty("hooks").value<QObject*>()
        );

        if ( !hooks )
            return nullptr;

        QList<QObject*> watchers = hooks->entriesFor(wnode->filePath, wnode->objectId);

        if ( watchers.isEmpty() )
            return nullptr;

        QmlWatcher* watcher = qobject_cast<QmlWatcher*>(watchers.first());
        viewRoot = watcher->parent();

        root = root->child;
    }

    while ( root && (root->type() == QmlBindingPath::Node::File || root->type() == QmlBindingPath::Node::Component) ){
        root = root->child;
    }

    if ( !root ){
        if ( viewRoot ){
            return QmlBindingChannel::create(path, r, viewRoot);
        }
        return nullptr;
    }

    if ( root->child == nullptr ){
        if ( root->type() == QmlBindingPath::Node::Index ){
            QmlBindingPath::IndexNode* inode = static_cast<QmlBindingPath::IndexNode*>(root);

            QQmlProperty prop(viewRoot);
            if( !prop.isValid() )
                return QmlBindingChannel::Ptr();
            return QmlBindingChannel::create(path, r, prop, inode->index);

        } else if ( root->type() == QmlBindingPath::Node::Property ){
            QmlBindingPath::PropertyNode* pnode = static_cast<QmlBindingPath::PropertyNode*>(root);
            QQmlProperty prop(viewRoot, pnode->propertyName);
            if( !prop.isValid() )
                return QmlBindingChannel::Ptr();
            return QmlBindingChannel::create(path, r, prop);
        }
    }

    return traversePath(path, r, root, viewRoot);
}

void DocumentQmlChannels::initialize(){

}

QStringList DocumentQmlChannels::describePath(const QmlBindingPath::Ptr &bp){
    QStringList result;

    QmlBindingPath::Node* cur = bp->root();
    while ( cur ){
        if ( cur->type() == QmlBindingPath::Node::Index ){
            QmlBindingPath::IndexNode* ci = static_cast<QmlBindingPath::IndexNode*>(cur);
            result.append("I/" + QString::number(ci->index));
        } else if ( cur->type() == QmlBindingPath::Node::Property ){
            QmlBindingPath::PropertyNode* cp = static_cast<QmlBindingPath::PropertyNode*>(cur);
            result.append("P/" + cp->propertyName);
        } else if ( cur->type() == QmlBindingPath::Node::File ){
            QmlBindingPath::FileNode* cf = static_cast<QmlBindingPath::FileNode*>(cur);
            result.append("F/" + cf->filePath.mid(cf->filePath.lastIndexOf('/') + 1));
        } else if ( cur->type() == QmlBindingPath::Node::Component ){
            QmlBindingPath::ComponentNode* cf = static_cast<QmlBindingPath::ComponentNode*>(cur);
            result.append("C/" + cf->name);
        } else if ( cur->type() == QmlBindingPath::Node::Watcher ){
            QmlBindingPath::WatcherNode* cf = static_cast<QmlBindingPath::WatcherNode*>(cur);
            result.append("W/" + cf->objectId);
        }
        cur = cur->child;
    }

    return result;
}

void DocumentQmlChannels::desyncInactiveChannels(){
    beginResetModel();
    for ( const QmlBindingChannel::Ptr& ch : m_channels ){
        if ( ch->object() && ch.data() != m_selectedChannel.data() ){
            ch->object()->setProperty("__connectionSync", false);
        }
    }
    endResetModel();
}

void DocumentQmlChannels::rebuild(){
    if ( !m_selectedChannel || !m_selectedChannel->isBuilder() ){
        return;
    }

    m_selectedChannel->rebuild();

}

}// namespace

