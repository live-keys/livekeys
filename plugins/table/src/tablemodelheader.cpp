#include "tablemodelheader.h"

namespace lv {


TableModelHeader::TableModelHeader(QObject *parent)
    : QAbstractListModel(parent)
    , m_defaultColumnWidth(100)
    , m_contentWidth(0)
{
    m_roles[Name] = "name";
}

int TableModelHeader::rowCount(const QModelIndex &) const{
    return 1;
}

int TableModelHeader::columnCount(const QModelIndex &) const{
    return m_data.size();
}

QVariant TableModelHeader::data(const QModelIndex &index, int) const{
    if (index.row() > 0)
        return QVariant();
    if (index.column() >= m_data.size())
        return QVariant();

    return m_data[index.column()].name;
}

bool TableModelHeader::setData(const QModelIndex &index, const QVariant &, int)
{
    if (index.row() > 0)
        return false;

    if (index.column() >= m_data.size()){
        beginInsertColumns(QModelIndex(), m_data.size(), index.column());
        for (int i = m_data.size(); i <= index.column(); ++i)
            m_data.push_back(createColumnData(i));
        endInsertColumns();
        return true;
    }

    m_data[index.column()] = createColumnData(index.column());
    emit dataChanged(index, index);

    return true;
}

void TableModelHeader::addColumn(const QString &name)
{
    beginInsertColumns(QModelIndex(), m_data.size(), m_data.size());
    auto modelData = createColumnData(m_data.size());
    if ( !name.isEmpty() )
        modelData.name = name;
    m_data.push_back(modelData);
    endInsertColumns();

}

void TableModelHeader::removeColumn(int idx)
{
    beginRemoveColumns(QModelIndex(), idx, idx);

    m_contentWidth -= m_data[idx].width;
    emit contentWidthChanged();
    m_data.erase(m_data.begin()+idx);

    endRemoveColumns();
}

void TableModelHeader::initalizeData(int size)
{
    if (size < 1)
        return;

    beginInsertColumns(QModelIndex(), 0, size-1);

    for (int i =0; i < size; ++i)
        m_data.push_back(createColumnData(i));
    endInsertColumns();
}

void TableModelHeader::notifyModelReset(){
    beginResetModel();
    m_data.clear();
    endResetModel();
    m_contentWidth = 0;
    emit contentWidthChanged();
}

int TableModelHeader::size() const{
    return m_data.size();
}

void TableModelHeader::updateColumnWidth(int index, int width){
    if ( index >= m_data.size() )
        return;
    int delta = width - m_data[index].width;
    m_data[index].width = width;
    m_contentWidth += delta;
    if (delta != 0)
        emit contentWidthChanged();
}

int TableModelHeader::columnWidth(int index){
    if ( index >= m_data.size() )
        return m_defaultColumnWidth;
    return m_data[index].width;
}

TableModelHeader::ColumnData TableModelHeader::createColumnData(int idx){
    int rem = idx%26;
    QString name = QString(static_cast<char>(static_cast<int>('A') + rem));
    idx /= 26;

    while (idx){
        int rem = idx%26;
        idx = idx/26;
        name = QString(static_cast<char>(static_cast<int>('A') + rem - 1)) + name;
    }

    TableModelHeader::ColumnData data;
    data.name = name;
    data.width = m_defaultColumnWidth;
    m_contentWidth += m_defaultColumnWidth;
    emit contentWidthChanged();
    return data;
}

int TableModelHeader::contentWidth() const
{
    return m_contentWidth;
}

void TableModelHeader::assignColumnName(int index, const QString &name){
    if ( index >= m_data.length() )
        return;
    m_data[index].name = name;
    emit dataChanged(createIndex(0, index), createIndex(0, index));
}

}

