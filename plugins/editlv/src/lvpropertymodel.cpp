#include "lvpropertymodel.h"
#include "live/visuallogqt.h"

#include <QSet>

namespace lv{

LvPropertyModel::LvPropertyModel(int addPosition, QObject *parent)
    : QAbstractListModel(parent)
    , m_addPosition(addPosition)
{
    m_roles[LvPropertyModel::Label]         = "label";
    m_roles[LvPropertyModel::ObjectType]    = "objectType";
    m_roles[LvPropertyModel::Type]          = "type";
    m_roles[LvPropertyModel::Documentation] = "documentation";
    m_roles[LvPropertyModel::Code]          = "code";
}

LvPropertyModel::~LvPropertyModel(){
}

QVariant LvPropertyModel::data(const QModelIndex &index, int role) const{
    int dataIndex = m_filteredData[index.row()];
    if ( role == LvPropertyModel::Label ){
        return m_data[dataIndex].label;
    } else if ( role == LvPropertyModel::Type ){
        return m_data[dataIndex].type;
    } else if ( role == LvPropertyModel::ObjectType ){
        return m_data[dataIndex].objectType;
    } else if ( role == LvPropertyModel::Documentation ){
        return m_data[dataIndex].documentation;
    } else if ( role == LvPropertyModel::Code ){
        return m_data[dataIndex].code;
    }
    return QVariant();
}

void LvPropertyModel::addItem(const LvPropertyModel::PropertyData &item){
    m_data.append(item);
}

void LvPropertyModel::setFilter(const QString &filter){
    m_filter = filter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

void LvPropertyModel::setTypeFilter(const QString &typeFilter){
    if ( m_typeFilter == typeFilter )
        return;

    m_typeFilter = typeFilter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

QStringList LvPropertyModel::types() const{
    QSet<QString> types;
    for ( auto it = m_data.begin(); it != m_data.end(); ++it ){
        types.insert(it->objectType);
    }

    QStringList res;
    res << "All";
    foreach( const QString& c, types ){
        res << c;
    }
    return res;
}

void LvPropertyModel::updateFilters(){
    m_filteredData.clear();
    for ( int i = 0; i < m_data.size(); ++i ){
        bool filter = m_data[i].label.startsWith(m_filter, Qt::CaseInsensitive);
        bool typeFilter = m_typeFilter.isEmpty() ? true : m_data[i].objectType == m_typeFilter;

        if ( filter && typeFilter )
            m_filteredData.append(i);
    }
}

LvPropertyModel::PropertyData::PropertyData(
        const QString &plabel,
        const QString &pobjectType,
        const QString &ptype,
        const QString &pdoc,
        const QString &pcode)
    : label(plabel)
    , objectType(pobjectType)
    , type(ptype)
    , documentation(pdoc)
    , code(pcode)
{
}

}// namespace
