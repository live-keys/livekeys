#include "qmlsuggestionmodel.h"
#include <QSet>

namespace lv {

QmlSuggestionModel::QmlSuggestionModel(int addPosition, QObject* parent):
    QAbstractListModel(parent),
    m_addPosition(addPosition)
{
    m_roles[Label]          = "label";
    m_roles[ObjectType]     = "objectType";
    m_roles[Type]           = "type";
    m_roles[ImportSpace]    = "importSpace";
    m_roles[Documentation]  = "documentation";
    m_roles[Code]           = "code";
}

QmlSuggestionModel::~QmlSuggestionModel()
{

}

QVariant QmlSuggestionModel::data(const QModelIndex &index, int role) const
{
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
    } else if ( role == ImportSpace ){
        return m_data[dataIndex].importSpace;
    }
    return QVariant();
}

void QmlSuggestionModel::addItem(const QmlSuggestionModel::ItemData &item)
{
    m_data.append(item);
}

void QmlSuggestionModel::setFilter(const QString &filter)
{
    m_filter = filter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

void QmlSuggestionModel::setImportFilter(const QString &importFilter)
{
    if ( m_importFilter == importFilter )
        return;

    m_importFilter = importFilter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

void QmlSuggestionModel::setTypeFilter(const QString &typeFilter)
{
    if ( m_typeFilter == typeFilter )
        return;

    m_typeFilter = typeFilter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

QStringList QmlSuggestionModel::importSpaces() const
{
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

QStringList QmlSuggestionModel::types() const{
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

void QmlSuggestionModel::updateFilters(){
    m_filteredData.clear();
    for ( int i = 0; i < m_data.size(); ++i ){
        bool filter = m_data[i].label.startsWith(m_filter, Qt::CaseInsensitive);
        bool typeFilter = m_typeFilter.isEmpty() ? true : m_data[i].objectType == m_typeFilter;
        bool importFilter = m_importFilter.isEmpty() ? true : m_data[i].importSpace == m_importFilter;

        if ( filter && typeFilter && importFilter)
            m_filteredData.append(i);
    }
}


QmlSuggestionModel::ItemData::ItemData(const QString& plabel, const QString& pObjType, const QString& ptype, const QString &pimport, const QString &pdoc, const QString &pcode)
    : label(plabel)
    , objectType(pObjType)
    , type(ptype)
    , importSpace(pimport)
    , documentation(pdoc)
    , code(pcode)
{

}

}
