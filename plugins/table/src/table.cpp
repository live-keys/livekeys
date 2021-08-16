#include "table.h"
#include <QDebug>
#include "tableheader.h"
#include "tablerowsinfo.h"
#include "localdatasource.h"

namespace lv {

Table::Table(QObject *parent)
    : QAbstractTableModel(parent)
    , m_isComponentComplete(false)
    , m_headerModel(new TableHeader(this))
    , m_rowModel(new TableRowsInfo(this))
    , m_dataSource(new LocalDataSource(this))
{
    m_roles[Value]      = "value";
    m_roles[IsSelected] = "isSelected";
}

Table::~Table(){
    delete m_headerModel;
    delete m_rowModel;
}

int Table::rowCount(const QModelIndex &) const{
    return m_dataSource->totalRows();
}

int Table::columnCount(const QModelIndex &) const{
    return m_headerModel->size();
}

Qt::ItemFlags Table::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}


QVariant Table::data(const QModelIndex &index, int role) const{
    if ( index.row() >= m_dataSource->totalRows() || index.column() >= columnCount() ){
        return QVariant();
    }

    if ( role == Table::Value ){
        return m_dataSource->valueAt(index.row(), index.column());
    } else if ( role == Table::IsSelected){
        return m_rowModel->isSelected(index.column(), index.row());
    }
    return QVariant();
}

bool Table::setData(const QModelIndex &index, const QVariant &value, int)
{
    if ( index.row() < m_dataSource->totalRows() && index.column() < columnCount() ){

        m_dataSource->setValueAt(index.row(), index.column(), value.toString());

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
        m_dataSource->addRow();
        m_rowModel->notifyRowAdded();
    }

    endInsertRows();
}

void Table::addColumns(int number){
    int colIndex = m_headerModel->size();
    beginInsertColumns(QModelIndex(), colIndex, colIndex + number - 1);

    for ( int newCols = 0; newCols < number; ++newCols ){
        m_dataSource->addColumn();
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
    m_dataSource->removeColumn(idx);
//    for (int i = 0; i < m_data.size(); ++i){
//        if (idx >= m_data[i].size())
//            continue;
//        m_data[i].erase(m_data[i].begin() + idx);
//    }
    m_headerModel->removeColumn();
    endRemoveColumns();
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

void Table::setDataSource(TableDataSource *dataSource)
{
    if (m_dataSource == dataSource)
        return;

    m_dataSource = dataSource;
    emit dataSourceChanged();
}

} // namespace
