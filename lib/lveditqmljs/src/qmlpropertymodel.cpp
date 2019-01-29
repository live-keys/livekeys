#include "qmlpropertymodel.h"
#include "live/visuallogqt.h"

#include <QSet>

namespace lv{

QmlPropertyModel::QmlPropertyModel(int addPosition, QObject *parent)
    : QAbstractListModel(parent)
    , m_addPosition(addPosition)
{
    m_roles[QmlPropertyModel::Label]         = "label";
    m_roles[QmlPropertyModel::ObjectType]    = "objectType";
    m_roles[QmlPropertyModel::Type]          = "type";
    m_roles[QmlPropertyModel::Documentation] = "documentation";
    m_roles[QmlPropertyModel::Code]          = "code";
}

QmlPropertyModel::~QmlPropertyModel(){
}

QVariant QmlPropertyModel::data(const QModelIndex &index, int role) const{
    int dataIndex = m_filteredData[index.row()];
    if ( role == QmlPropertyModel::Label ){
        return m_data[dataIndex].label;
    } else if ( role == QmlPropertyModel::Type ){
        return m_data[dataIndex].type;
    } else if ( role == QmlPropertyModel::ObjectType ){
        return m_data[dataIndex].objectType;
    } else if ( role == QmlPropertyModel::Documentation ){
        return m_data[dataIndex].documentation;
    } else if ( role == QmlPropertyModel::Code ){
        return m_data[dataIndex].code;
    }
    return QVariant();
}

void QmlPropertyModel::addItem(const QmlPropertyModel::PropertyData &item){
    m_data.append(item);
}

void QmlPropertyModel::setFilter(const QString &filter){
    m_filter = filter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

void QmlPropertyModel::setTypeFilter(const QString &typeFilter){
    if ( m_typeFilter == typeFilter )
        return;

    m_typeFilter = typeFilter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

QStringList QmlPropertyModel::types() const{
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

void QmlPropertyModel::updateFilters(){
    m_filteredData.clear();
    for ( int i = 0; i < m_data.size(); ++i ){
        bool filter = m_data[i].label.startsWith(m_filter, Qt::CaseInsensitive);
        bool typeFilter = m_typeFilter.isEmpty() ? true : m_data[i].objectType == m_typeFilter;

        if ( filter && typeFilter )
            m_filteredData.append(i);
    }
}

QmlPropertyModel::PropertyData::PropertyData(
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
