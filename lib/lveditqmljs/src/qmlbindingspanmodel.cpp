#include "qmlbindingspanmodel.h"
#include "qmleditfragment.h"
#include "qmlusagegraphscanner.h"
#include "codeqmlhandler.h"
#include "live/projectfile.h"

namespace lv{

QmlBindingSpanModel::QmlBindingSpanModel(QmlEditFragment *edit, QObject *parent)
    : QAbstractListModel(parent)
    , m_edit(edit)
{
    m_roles[QmlBindingSpanModel::Path]         = "path";
    m_roles[QmlBindingSpanModel::IsConnected]  = "isConnected";
    m_roles[QmlBindingSpanModel::RunnableName] = "runnableName";
}

QmlBindingSpanModel::~QmlBindingSpanModel(){
}

int QmlBindingSpanModel::rowCount(const QModelIndex &) const{
    return m_edit->bindingSpan()->channels().size();
}

QVariant QmlBindingSpanModel::data(const QModelIndex &index, int role) const{
    QmlBindingSpan* b = m_edit->bindingSpan();
    if ( index.row() < b->channels().size() ){
        if ( role == QmlBindingSpanModel::Path ){
            QStringList cp = createPath(b->channels().at(index.row())->bindingPath());
            return cp;
        } else if ( role == QmlBindingSpanModel::IsConnected ){
            return b->channels().at(index.row())->isEnabled();
        } else if ( role == QmlBindingSpanModel::RunnableName ){
            QString path = b->channels().at(index.row())->runnable()->path();
            return path.mid(path.lastIndexOf('/') + 1);
        }
    }

    return QVariant();
}

void QmlBindingSpanModel::connectPathAtIndex(int index){
    QmlBindingSpan* bspan = m_edit->bindingSpan();
    int currentIndex = inputPathIndex();
    if ( currentIndex != index && index < bspan->channels().size() ){
        bspan->setConnectionChannel(bspan->channels()[index]);
        //TODO: Propagate to all child edits
        emit inputPathIndexChanged(index);
    }
}

QStringList QmlBindingSpanModel::createPath(const QmlBindingPath::Ptr &bp) const{
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
        }
        cur = cur->child;
    }

    return result;
}

int QmlBindingSpanModel::inputPathIndex() const{
    QmlBindingSpan* b = m_edit->bindingSpan();
    if ( !b->connectionChannel() )
        return -1;

    for ( int i = 0; i < b->channels().size(); ++i ){
        if ( b->channels()[i] == b->connectionChannel() ){
            return i;
        }
    }
    return -1;
}

}// namespace
