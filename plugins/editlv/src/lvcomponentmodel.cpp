#include "lvcomponentmodel.h"
#include <QSet>

namespace lv{

LvComponentModel::LvComponentModel(int addPosition, QObject *parent)
    : QAbstractListModel(parent)
    , m_addPosition(addPosition)
{
    m_roles[LvComponentModel::Label]         = "label";
    m_roles[LvComponentModel::ImportSpace]   = "importSpace";
    m_roles[LvComponentModel::Documentation] = "documentation";
    m_roles[LvComponentModel::Code]          = "code";
}

LvComponentModel::~LvComponentModel(){
}

QVariant LvComponentModel::data(const QModelIndex &index, int role) const{
    int dataIndex = m_filteredData[index.row()];
    if ( role == LvComponentModel::Label ){
        return m_data[dataIndex].label;
    } else if ( role == LvComponentModel::ImportSpace ){
        return m_data[dataIndex].importSpace;
    } else if ( role == LvComponentModel::Documentation ){
        return m_data[dataIndex].documentation;
    } else if ( role == LvComponentModel::Code ){
        return m_data[dataIndex].code;
    }
    return QVariant();
}

void LvComponentModel::addItem(const LvComponentModel::ComponentData &item){
    m_data.append(item);
}

void LvComponentModel::setFilter(const QString &filter){
    m_filter = filter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

void LvComponentModel::setImportFilter(const QString &importFilter){
    if ( importFilter == m_importFilter )
        return;

    m_importFilter = importFilter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

void LvComponentModel::updateFilters(){
    m_filteredData.clear();
    for ( int i = 0; i < m_data.size(); ++i ){
        bool filter = m_data[i].label.startsWith(m_filter, Qt::CaseInsensitive);
        bool importFilter = m_importFilter.isEmpty() ? true : m_data[i].importSpace == m_importFilter;

        if ( filter && importFilter )
            m_filteredData.append(i);
    }
}

QStringList LvComponentModel::importSpaces(){
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

LvComponentModel::ComponentData::ComponentData(
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
