#include "qmleventmodel.h"
#include "live/visuallogqt.h"

#include <QSet>

namespace lv{

QmlEventModel::QmlEventModel(int addPosition, QObject *parent)
    : QAbstractListModel(parent)
    , m_addPosition(addPosition)
{
    m_roles[Label]         = "label";
    m_roles[ObjectType]    = "objectType";
    m_roles[Type]          = "type";
    m_roles[Documentation] = "documentation";
    m_roles[Code]          = "code";
}

QmlEventModel::~QmlEventModel(){
}

QVariant QmlEventModel::data(const QModelIndex &index, int role) const{
    int dataIndex = m_filteredData[index.row()];
    if ( role == Label ){
        return m_data[dataIndex].label;
    } else if ( role == Type ){
        return m_data[dataIndex].type;
    } else if ( role == ObjectType ){
        return m_data[dataIndex].objectType;
    } else if ( role == Documentation ){
        return m_data[dataIndex].documentation;
    } else if ( role == Code ){
        return m_data[dataIndex].code;
    }
    return QVariant();
}

void QmlEventModel::addItem(const EventData &item){
    m_data.append(item);
}

void QmlEventModel::setFilter(const QString &filter){
    m_filter = filter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

void QmlEventModel::setTypeFilter(const QString &typeFilter){
    if ( m_typeFilter == typeFilter )
        return;

    m_typeFilter = typeFilter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

QStringList QmlEventModel::types() const{
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

void QmlEventModel::updateFilters(){
    m_filteredData.clear();
    for ( int i = 0; i < m_data.size(); ++i ){
        bool filter = m_data[i].label.startsWith(m_filter, Qt::CaseInsensitive);
        bool typeFilter = m_typeFilter.isEmpty() ? true : m_data[i].objectType == m_typeFilter;

        if ( filter && typeFilter )
            m_filteredData.append(i);
    }
}

QmlEventModel::EventData::EventData(
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
