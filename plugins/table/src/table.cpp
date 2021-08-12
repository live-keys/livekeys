#include "table.h"
#include <QDebug>
#include "tableheader.h"
#include "tablerows.h"

namespace lv {

Table::Table(QObject *parent)
    : QAbstractTableModel(parent)
    , m_headerModel(new TableHeader(this))
    , m_rowModel(new TableRows(this))
{
    m_roles[Value] = "value";
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
        return QVariant();
    }
    auto& row = m_data[index.row()];
    if (index.column() >= columnCount()){
        return QVariant();
    }
    if (index.column() >= row.size()){
        return QVariant("");
    }
    return row[index.column()];
}

bool Table::setData(const QModelIndex &index, const QVariant &value, int)
{
    if (index.row() >= m_data.size()){
        for (int i = m_data.size(); i<=index.row();++i)
            addRow();
    }

    int oldColumnCount = columnCount();

    if (index.column() >= oldColumnCount){
        for (int i = oldColumnCount; i<=index.column();++i)
            addColumn();
    }
    m_data[index.row()][index.column()] = value.toString();
    emit dataChanged(index, index);

    return true;
}

void Table::componentComplete()
{
    emit complete();
}

TableHeader *Table::headerModel() const
{
    return m_headerModel;
}

TableRows *Table::rowModel() const
{
    return m_rowModel;
}

void Table::addRow()
{
    int rowIndex = rowCount();

    beginInsertRows(QModelIndex(), rowIndex, rowIndex);
    m_data.push_back({});

    int colNum = columnCount();
    for (int i=0; i < colNum; ++i)
        m_data[rowIndex].push_back("");
    endInsertRows();

    m_rowModel->addRow();
}

void Table::addColumn()
{
    int colIndex = columnCount();
    beginInsertColumns(QModelIndex(), colIndex, colIndex);

    int rowNum = rowCount();
    for (int i = 0; i < rowNum; ++i)
        m_data[i].push_back("");
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

void Table::assignCell(int row, int col, QString value)
{
    setData(QAbstractItemModel::createIndex(row, col), value);
}

} // namespace
