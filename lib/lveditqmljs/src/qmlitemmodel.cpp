#include "qmlitemmodel.h"
#include <QSet>

namespace lv{

QmlItemModel::QmlItemModel(int addPosition, QObject *parent)
    : QAbstractListModel(parent)
    , m_addPosition(addPosition)
{
    m_roles[QmlItemModel::Label]         = "label";
    m_roles[QmlItemModel::ImportSpace]   = "importSpace";
    m_roles[QmlItemModel::Documentation] = "documentation";
    m_roles[QmlItemModel::Code]          = "code";
}

QmlItemModel::~QmlItemModel(){
}

QVariant QmlItemModel::data(const QModelIndex &index, int role) const{
    int dataIndex = m_filteredData[index.row()];
    if ( role == QmlItemModel::Label ){
        return m_data[dataIndex].label;
    } else if ( role == QmlItemModel::ImportSpace ){
        return m_data[dataIndex].importSpace;
    } else if ( role == QmlItemModel::Documentation ){
        return m_data[dataIndex].documentation;
    } else if ( role == QmlItemModel::Code ){
        return m_data[dataIndex].code;
    }
    return QVariant();
}

void QmlItemModel::addItem(const QmlItemModel::ItemData &item){
    m_data.append(item);
}

void QmlItemModel::setFilter(const QString &filter){
    m_filter = filter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

void QmlItemModel::setImportFilter(const QString &importFilter){
    if ( importFilter == m_importFilter )
        return;

    m_importFilter = importFilter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

void QmlItemModel::updateFilters(){
    m_filteredData.clear();
    for ( int i = 0; i < m_data.size(); ++i ){
        bool filter = m_data[i].label.startsWith(m_filter, Qt::CaseInsensitive);
        bool importFilter = m_importFilter.isEmpty() ? true : m_data[i].importSpace == m_importFilter;

        if ( filter && importFilter )
            m_filteredData.append(i);
    }
}

QStringList QmlItemModel::importSpaces(){
    QSet<QString> cat;
    for ( auto it = m_data.begin(); it != m_data.end(); ++it ){
        cat.insert(it->importSpace);
    }

    QStringList res;
    res << "All";
    foreach( const QString& c, cat ){
        res << c;
    }
    return res;
}

QmlItemModel::ItemData::ItemData(
        const QString plabel,
        const QString &pimport,
        const QString &pdoc,
        const QString &pcode)
    : label(plabel)
    , importSpace(pimport)
    , documentation(pdoc)
    , code(pcode)
{
}

}// namespace
