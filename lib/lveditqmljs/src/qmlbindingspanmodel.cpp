#include "qmlbindingspanmodel.h"
#include "qmleditfragment.h"
#include "qmlusagegraphscanner.h"
#include "codeqmlhandler.h"
#include "live/projectfile.h"

namespace lv{

QmlBindingSpanModel::QmlBindingSpanModel(QmlEditFragment *edit, QObject *parent)
    : QAbstractListModel(parent)
    , m_edit(edit)
    , m_scanner(nullptr)
    , m_isScanning(false)
{
    m_roles[QmlBindingSpanModel::Path]         = "path";
    m_roles[QmlBindingSpanModel::IsConnected]  = "isConnected";
    m_roles[QmlBindingSpanModel::RunnableName] = "runnableName";
}

QmlBindingSpanModel::~QmlBindingSpanModel(){
    if ( m_scanner ){
        m_scanner->requestStop();
        if ( !m_scanner->wait(100) ){
            m_scanner->terminate();
            m_scanner->wait();
        }
        delete m_scanner;
    }
}

int QmlBindingSpanModel::rowCount(const QModelIndex &) const{
    return m_edit->bindingSpan()->outputChannels().size();
}

QVariant QmlBindingSpanModel::data(const QModelIndex &index, int role) const{
    QmlBindingSpan* b = m_edit->bindingSpan();
    if ( index.row() < b->outputChannels().size() ){
        if ( role == QmlBindingSpanModel::Path ){
            QStringList cp = createPath(b->outputChannels().at(index.row())->bindingPath());
            return cp;
        } else if ( role == QmlBindingSpanModel::IsConnected ){
            return b->outputChannels().at(index.row())->isEnabled();
        } else if ( role == QmlBindingSpanModel::RunnableName ){
            QString path = b->outputChannels().at(index.row())->runnable()->path();
            return path.mid(path.lastIndexOf('/') + 1);
        }
    }

    return QVariant();
}

void QmlBindingSpanModel::makePathInput(int index){
    QmlBindingSpan* bspan = m_edit->bindingSpan();
    int currentIndex = inputPathIndex();
    if ( currentIndex != index && index < bspan->outputChannels().size() ){
        bspan->setInputChannel(bspan->outputChannels()[index]);
        //TODO: Propagate to all child edits
        emit inputPathIndexChanged(index);
    }
}

void QmlBindingSpanModel::setPathConnection(int index, bool connection){
    QmlBindingSpan* bspan = m_edit->bindingSpan();

    if ( index >= bspan->outputChannels().size() )
        return;

    QmlBindingChannel::Ptr c = bspan->outputChannels().at(index);
    if ( c->isEnabled() != connection ){
        c->setEnabled(connection);

        emit dataChanged(createIndex(index, 0), createIndex(index, 0));
        //TODO: Propagate to all child edits
        emit pathConnectionChanged(index, connection);
    }
}

void QmlBindingSpanModel::__scannerBindingPathAdded(){
    QPair<Runnable*, QmlBindingPath::Ptr> rbp = m_scanner->popResult();
    Runnable* r = rbp.first;
    QmlBindingPath::Ptr bp = rbp.second;

    QmlBindingChannel::Ptr bc = QmlBindingChannel::create(bp, r);
    QmlBindingSpan* b = m_edit->bindingSpan();

    beginInsertRows(QModelIndex(), b->outputChannels().size(), b->outputChannels().size());
    b->addOutputChannel(bc);

    endInsertRows();
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
    if ( !b->inputChannel() )
        return -1;

    for ( int i = 0; i < b->outputChannels().size(); ++i ){
        if ( b->outputChannels()[i] == b->inputChannel() ){
            return i;
        }
    }
    return -1;
}

bool QmlBindingSpanModel::isScanning() const{
    return m_isScanning;
}

void QmlBindingSpanModel::initializeScanner(CodeQmlHandler* qmlHandler){
    m_scanner = qmlHandler->createScanner();

    connect(m_scanner, &QmlUsageGraphScanner::bindingPathAdded, this, &QmlBindingSpanModel::__scannerBindingPathAdded);

    QString componentPath = m_edit->declaration()->document()->file()->path();
    QString componentName = m_edit->declaration()->document()->file()->name();
    componentName = componentName.mid(0, componentName.indexOf('.'));

    m_scanner->setSearchComponent(componentPath, componentName);
    m_scanner->start();
}

}// namespace
