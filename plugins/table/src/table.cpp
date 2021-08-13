#include "table.h"
#include <QDebug>
#include "tableheader.h"
#include "tablerowsinfo.h"

namespace lv {

Table::Table(QObject *parent)
    : QAbstractTableModel(parent)
    , m_isComponentComplete(false)
    , m_headerModel(new TableHeader(this))
    , m_rowModel(new TableRowsInfo(this))
{
    m_roles[Value]      = "value";
    m_roles[IsSelected] = "isSelected";
}

Table::~Table(){
    delete m_headerModel;
    delete m_rowModel;
}

int Table::rowCount(const QModelIndex &) const
{
    return m_data.size();
}

int Table::columnCount(const QModelIndex &) const{
    return m_headerModel->size();
}

Qt::ItemFlags Table::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}


QVariant Table::data(const QModelIndex &index, int role) const{
    if ( index.row() >= m_data.size() || index.column() >= columnCount() ){
        return QVariant();
    }

    if ( role == Table::Value ){
        auto& row = m_data[index.row()];
        return row[index.column()];
    } else if ( role == Table::IsSelected){
        return m_rowModel->isSelected(index.column(), index.row());
    }
    return QVariant();
}

bool Table::setData(const QModelIndex &index, const QVariant &value, int)
{
    if ( index.row() < m_data.size() && index.column() < columnCount() ){
        m_data[index.row()][index.column()] = value.toString();

        emit dataChanged(index, index);

        return true;
    }
    return false;
}

void Table::componentComplete()
{
    m_isComponentComplete = true;
    emit complete();
}

TableHeader *Table::header() const
{
    return m_headerModel;
}

TableRowsInfo *Table::rowInfo() const
{
    return m_rowModel;
}

void Table::addRows(int number)
{
    int rowIndex = rowCount();

    beginInsertRows(QModelIndex(), rowIndex, rowIndex + number - 1);

    for ( int newRows = 0; newRows < number; ++newRows ){
        m_data.push_back({});

        int colNum = m_headerModel->size();
        for (int i=0; i < colNum; ++i)
            m_data[rowIndex + newRows].push_back("");

        m_rowModel->notifyRowAdded();
    }

    endInsertRows();
}

void Table::addColumns(int number){
    int colIndex = m_headerModel->size();
    beginInsertColumns(QModelIndex(), colIndex, colIndex + number - 1);

    for ( int newCols = 0; newCols < number; ++newCols ){
        int rowNum = rowCount();
        for (int i = 0; i < rowNum; ++i)
            m_data[i].push_back("");
        m_headerModel->addColumn();
        m_rowModel->notifyColumnAdded();
    }

    endInsertColumns();
}

void Table::removeColumn(int idx)
{
    if (idx >= columnCount())
        return;

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

bool Table::select(QJSValue column, QJSValue row){
    if ( column.isNumber() && row.isNumber() ){
        m_rowModel->select(column.toInt(), row.toInt());
        auto index = QAbstractItemModel::createIndex(row.toInt(), column.toInt());
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool Table::deselect(QJSValue column, QJSValue row){
    if ( column.isUndefined() && row.isUndefined() ){
        beginResetModel();
        m_rowModel->deselectAll();
        endResetModel();
        return true;
    }
    return false;
}

} // namespace
