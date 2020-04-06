#include "bindingspanmodel.h"
#include "lveditfragment.h"
// #include "qmlusagegraphscanner.h" // TODO: ELEMENTS
#include "languagelvhandler.h"
#include "live/projectfile.h"

namespace lv{

BindingSpanModel::BindingSpanModel(LvEditFragment *edit, QObject *parent)
    : QAbstractListModel(parent)
    , m_edit(edit)
    //, m_scanner(nullptr)
    , m_isScanning(false)
{
    m_roles[Path]         = "path";
    m_roles[IsConnected]  = "isConnected";
    m_roles[RunnableName] = "runnableName";
}

BindingSpanModel::~BindingSpanModel(){
//    if ( m_scanner ){
//        m_scanner->requestStop();
//        if ( !m_scanner->wait(100) ){
//            m_scanner->terminate();
//            m_scanner->wait();
//        }
//        delete m_scanner;
//    } // TODO: ELEMENTS #382
}

int BindingSpanModel::rowCount(const QModelIndex &) const{
    return m_edit->bindingSpan()->channels().size();
}

QVariant BindingSpanModel::data(const QModelIndex &index, int role) const{
    BindingSpan* b = m_edit->bindingSpan();
    if ( index.row() < b->channels().size() ){
        if ( role == BindingSpanModel::Path ){
            QStringList cp = createPath(b->channels().at(index.row())->bindingPath());
            return cp;
        } else if ( role == BindingSpanModel::IsConnected ){
            return b->channels().at(index.row())->isEnabled();
        } else if ( role == BindingSpanModel::RunnableName ){
            QString path = b->channels().at(index.row())->runnable()->path();
            return path.mid(path.lastIndexOf('/') + 1);
        }
    }

    return QVariant();
}

void BindingSpanModel::makePathInput(int index){
    BindingSpan* bspan = m_edit->bindingSpan();
    int currentIndex = inputPathIndex();
    if ( currentIndex != index && index < bspan->channels().size() ){
        bspan->connectChannel(bspan->channels()[index]);
        //TODO: Propagate to all child edits
        emit inputPathIndexChanged(index);
    }
}

void BindingSpanModel::setPathConnection(int index, bool connection){
    BindingSpan* bspan = m_edit->bindingSpan();

    if ( index >= bspan->channels().size() )
        return;

    BindingChannel::Ptr c = bspan->channels().at(index);
    if ( c->isEnabled() != connection ){
        c->setEnabled(connection);

        emit dataChanged(createIndex(index, 0), createIndex(index, 0));
        //TODO: Propagate to all child edits
        emit pathConnectionChanged(index, connection);
    }
}

void BindingSpanModel::__scannerBindingPathAdded(){
    QPair<Runnable*, el::BindingPath::Ptr> rbp;// = m_scanner->popResult(); // TODO: ELEMENTS #382
    Runnable* r = rbp.first;
    el::BindingPath::Ptr bp = rbp.second;

    BindingChannel::Ptr bc = BindingChannel::create(bp, r);
    BindingSpan* b = m_edit->bindingSpan();

    beginInsertRows(QModelIndex(), b->channels().size(), b->channels().size());
    b->connectChannel(bc);

    endInsertRows();
}

QStringList BindingSpanModel::createPath(const el::BindingPath::Ptr &bp) const{
    QStringList result;

    el::BindingPath::Node* cur = bp->root();
    while ( cur ){
        if ( cur->type() == el::BindingPath::Node::Index ){
            el::BindingPath::IndexNode* ci = static_cast<el::BindingPath::IndexNode*>(cur);
            result.append("I/" + QString::number(ci->index));
        } else if ( cur->type() == el::BindingPath::Node::Property ){
            el::BindingPath::PropertyNode* cp = static_cast<el::BindingPath::PropertyNode*>(cur);
            result.append("P/" + QString(cp->propertyName.c_str()));
        } else if ( cur->type() == el::BindingPath::Node::File ){
            el::BindingPath::FileNode* cf = static_cast<el::BindingPath::FileNode*>(cur);
            result.append("F/" + QString(cf->filePath.substr(cf->filePath.find_last_of('/') + 1).c_str()));
        } else if ( cur->type() == el::BindingPath::Node::Component ){
            el::BindingPath::ComponentNode* cf = static_cast<el::BindingPath::ComponentNode*>(cur);
            result.append("C/" + QString(cf->name.c_str()));
        }
        cur = cur->child;
    }

    return result;
}

int BindingSpanModel::inputPathIndex() const{
    BindingSpan* b = m_edit->bindingSpan();
    if ( !b->connectedChannel() )
        return -1;

    for ( int i = 0; i < b->channels().size(); ++i ){
        if ( b->channels()[i] == b->connectedChannel() ){
            return i;
        }
    }
    return -1;
}

bool BindingSpanModel::isScanning() const{
    return m_isScanning;
}

void BindingSpanModel::initializeScanner(LanguageLvHandler* lvHandler){
//    m_scanner = lvHandler->createScanner();

//    connect(m_scanner, &QmlUsageGraphScanner::bindingPathAdded, this, &BindingSpanModel::__scannerBindingPathAdded);
// TODO: ELEMENTS #382
    QString componentPath = m_edit->declaration()->document()->file()->path();
    QString componentName = m_edit->declaration()->document()->file()->name();
    componentName = componentName.mid(0, componentName.indexOf('.'));

//    m_scanner->setSearchComponent(componentPath, componentName);
//    m_scanner->start();
// TODO: ELEMENTS #382
}

}// namespace
