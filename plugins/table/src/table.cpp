#include "table.h"
#include <QDebug>

namespace lv {

Table::Table(QObject *parent)
    : QAbstractTableModel(parent)
    , m_headerModel(new TableHeader(this))
{
    m_roles[Value] = "value";
    initializeData();
}

Table::~Table()
{

}

int Table::rowCount(const QModelIndex &) const
{
    return m_data.size();
}

int Table::columnCount(const QModelIndex &) const
{
    int max = 0;
    for (auto row: m_data)
        if (row.size()>max)
            max = row.size();

    return max;
}

Qt::ItemFlags Table::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}


QVariant Table::data(const QModelIndex &index, int) const
{
    if (index.row() >= m_data.size()){
        return QVariant("");
    }
    auto& row = m_data[index.row()];
    if (index.column() >= row.size()){
        return QVariant("");
    }
    return row[index.column()];
}

bool Table::setData(const QModelIndex &index, const QVariant &value, int)
{
    if (index.row() >= m_data.size()){
        return false;
    }

    if (index.column() >= m_data[index.row()].size()){
        int colCount = std::max(columnCount(), index.column()+1);
        for (int i=m_data[index.row()].size(); i < colCount; ++i)
            m_data[index.row()].push_back("");
    }

    m_data[index.row()][index.column()] = value.toString();
    emit dataChanged(index, index);

    return true;
}

TableHeader *Table::headerModel() const
{
    return m_headerModel;
}

void Table::initializeData()
{
    beginInsertRows(QModelIndex(), 0, 3);
    m_data = {
        {"a", "b", "c"},
        {"d", "e"},
        {"g", "f", "h", "i", "j", "K"},
        {"g", "f", "h", "i", "j", "K"}
    };
    endInsertRows();

    m_headerModel->initalizeData(columnCount());
}

void Table::addRow()
{
    int rowIndex = rowCount();

    beginInsertRows(QModelIndex(), rowIndex, rowIndex);
    m_data.push_back({});

    int colNum = columnCount();
    for (int i=0; i < colNum; ++i)
        setData(QAbstractItemModel::createIndex(rowIndex, i), "");
    endInsertRows();
}

void Table::addColumn()
{
    int colIndex = columnCount();
    beginInsertColumns(QModelIndex(), colIndex, colIndex);

    int rowNum = rowCount();
    for (int i = 0; i < rowNum; ++i)
        setData(QAbstractItemModel::createIndex(i, colIndex), "");
    endInsertColumns();

    m_headerModel->addColumn();
}

void Table::removeColumn(int idx)
{
    if (idx >= columnCount()) return;

    beginRemoveColumns(QModelIndex(),idx, idx);
    for (int i=0; i<m_data.size(); ++i){
        if (idx >= m_data[i].size()) continue;
        m_data[i].erase(m_data[i].begin()+idx);
    }
    endRemoveColumns();
    m_headerModel->removeColumn();
}

} // namespace
