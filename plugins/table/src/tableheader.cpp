#include "tableheader.h"

namespace lv {


TableHeader::TableHeader(QObject *parent)
    : QAbstractListModel(parent)
    , m_defaultColumnWidth(100)
{
    m_roles[Name] = "name";
}

int TableHeader::rowCount(const QModelIndex &) const{
    return 1;
}

int TableHeader::columnCount(const QModelIndex &) const{
    return m_data.size();
}

QVariant TableHeader::data(const QModelIndex &index, int) const{
    if (index.row() > 0)
        return QVariant("");
    if (index.column() >= m_data.size())
        return QVariant("");

    return m_data[index.column()].name;
}

bool TableHeader::setData(const QModelIndex &index, const QVariant &, int)
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

    return true;
}

void TableHeader::addColumn()
{
    beginInsertColumns(QModelIndex(), m_data.size(), m_data.size());
    m_data.push_back(createColumnData(m_data.size()));
    endInsertColumns();

}

void TableHeader::removeColumn()
{
    beginRemoveColumns(QModelIndex(), m_data.size()-1, m_data.size()-1);
    m_data.pop_back();
    endRemoveColumns();
}

void TableHeader::initalizeData(int size)
{
    if (size < 1)
        return;

    beginInsertColumns(QModelIndex(), 0, size-1);

    for (int i =0; i < size; ++i)
        m_data.push_back(createColumnData(i));
    endInsertColumns();
}

void TableHeader::updateColumnWidth(int index, int width){
    if ( index >= m_data.size() )
        return;
    m_data[index].width = width;
}

int TableHeader::columnWidth(int index){
    if ( index >= m_data.size() )
        return m_defaultColumnWidth;
    return m_data[index].width;
}

TableHeader::ColumnData TableHeader::createColumnData(int idx){
    int rem = idx%26;
    QString name = QString(static_cast<char>(static_cast<int>('A') + rem));
    idx /= 26;

    while (idx){
        int rem = idx%26;
        idx = idx/26;
        name = QString(static_cast<char>(static_cast<int>('A') + rem - 1)) + name;
    }

    TableHeader::ColumnData data;
    data.name = name;
    data.width = m_defaultColumnWidth;
    return data;
}


}

