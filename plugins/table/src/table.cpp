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
    , m_dataSource(nullptr)
{
    m_roles[Value]      = "value";
    m_roles[IsSelected] = "isSelected";

    assignDataSource(new LocalDataSource(this));
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
    m_headerModel->removeColumn(idx);
    m_rowModel->removeColumn(idx);

    endRemoveColumns();
}

void Table::removeRow(int idx)
{
    if (idx >= rowCount())
        return;

    beginRemoveRows(QModelIndex(), idx, idx);
    m_dataSource->removeRow(idx);
    m_rowModel->removeRow(idx);

    endRemoveRows();
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

void Table::clearTable()
{
// TODO: Modify in accordance with m_dataSource
//    for (int i =0; i<m_data.size(); ++i)
//        for (int j=0; j<m_data[i].size(); ++j)
//            m_data[i][j] = "";
//    emit dataChanged(QAbstractItemModel::createIndex(0, 0),
//                     QAbstractItemModel::createIndex(m_data.size()-1, m_data[m_data.size()-1].size()-1));
}

void Table::__dataSourceAboutToLoad(){
    beginResetModel();
}

void Table::__dataSourceFinished(){
    m_rowModel->notifyModelReset(m_dataSource->totalRows());
    m_headerModel->notifyModelReset();

    auto names = m_dataSource->columnNames();

    for ( int i = 0; i < names.size(); ++i ){
        m_headerModel->addColumn(names[i]);
    }
    endResetModel();
}

void Table::assignDataSource(TableDataSource *ds){
    if ( m_dataSource == ds )
        return;
    //TODO: Reference counting on data source
    if ( m_dataSource){
        disconnect(m_dataSource, &TableDataSource::dataAboutToLoad, this, &Table::__dataSourceAboutToLoad);
        disconnect(m_dataSource, &TableDataSource::dataLoaded, this, &Table::__dataSourceFinished);
    }

    __dataSourceAboutToLoad();
    m_dataSource = ds;
    if ( m_dataSource ){
        connect(m_dataSource, &TableDataSource::dataAboutToLoad, this, &Table::__dataSourceAboutToLoad);
        connect(m_dataSource, &TableDataSource::dataLoaded, this, &Table::__dataSourceFinished);
    }
    __dataSourceFinished();
}

void Table::setDataSource(TableDataSource *dataSource)
{
    if (m_dataSource == dataSource)
        return;

    assignDataSource(dataSource);
    emit dataSourceChanged();
}

} // namespace
