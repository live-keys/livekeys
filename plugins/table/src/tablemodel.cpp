#include "tablemodel.h"
#include <QDebug>
#include "tablemodelheader.h"
#include "tablemodelrowsinfo.h"
#include "localtable.h"

namespace lv {

TableModel::TableModel(Table *parent)
    : QAbstractTableModel(parent)
    , m_isComponentComplete(false)
    , m_headerModel(new TableModelHeader(this))
    , m_rowModel(new TableModelRowsInfo(this))
    , m_dataSource(nullptr)
{
    m_roles[Value]      = "value";
    m_roles[IsSelected] = "isSelected";


    __dataSourceAboutToLoad();
    m_dataSource = parent;
    if ( m_dataSource ){
        connect(m_dataSource, &Table::dataAboutToLoad, this, &TableModel::__dataSourceAboutToLoad);
        connect(m_dataSource, &Table::dataLoaded, this, &TableModel::__dataSourceFinished);
    }
    __dataSourceFinished();
}

TableModel::~TableModel(){
    delete m_headerModel;
    delete m_rowModel;
}

int TableModel::rowCount(const QModelIndex &) const{
    return m_dataSource->totalRows();
}

int TableModel::columnCount(const QModelIndex &) const{
    return m_headerModel->size();
}

Qt::ItemFlags TableModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}


QVariant TableModel::data(const QModelIndex &index, int role) const{
    if ( index.row() >= m_dataSource->totalRows() || index.column() >= columnCount() ){
        return QVariant();
    }

    if ( role == TableModel::Value ){
        return m_dataSource->valueAt(index.row(), index.column());
    } else if ( role == TableModel::IsSelected){
        return m_rowModel->isSelected(index.column(), index.row());
    }
    return QVariant();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int)
{
    if ( index.row() < m_dataSource->totalRows() && index.column() < columnCount() ){

        m_dataSource->setValueAt(index.row(), index.column(), value.toString());

        emit dataChanged(index, index);

        return true;
    }
    return false;
}

void TableModel::componentComplete()
{
    m_isComponentComplete = true;
    emit complete();
}

TableModelHeader *TableModel::header() const
{
    return m_headerModel;
}

TableModelRowsInfo *TableModel::rowInfo() const
{
    return m_rowModel;
}

void TableModel::insertRow(QJSValue){
    //TODO
}

void TableModel::updateRow(int /*index*/, QJSValue /*values*/){
    //TODO
}

void TableModel::addRows(int number)
{
    int rowIndex = rowCount();

    beginInsertRows(QModelIndex(), rowIndex, rowIndex + number - 1);

    for ( int newRows = 0; newRows < number; ++newRows ){
        m_dataSource->addRow();
        m_rowModel->notifyRowAdded();
    }

    endInsertRows();
}

void TableModel::addColumns(int number){
    int colIndex = m_headerModel->size();
    beginInsertColumns(QModelIndex(), colIndex, colIndex + number - 1);

    for ( int newCols = 0; newCols < number; ++newCols ){
        m_dataSource->addColumn();
        m_headerModel->addColumn();
        m_rowModel->notifyColumnAdded();
    }

    endInsertColumns();
}

void TableModel::assignColumnInfo(int index, QJSValue name){
    m_headerModel->assignColumnName(index, name.toString());
    m_dataSource->assignColumnInfo(index, name.toString());
}

void TableModel::removeColumn(int idx)
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

void TableModel::removeRow(int index)
{
    if (index >= rowCount())
        return;

    beginRemoveRows(QModelIndex(), index, index);
    m_dataSource->removeRow(index);
    m_rowModel->removeRow(index);

    endRemoveRows();
}

int TableModel::totalRows() const{
    return m_dataSource->totalRows();
}

QJSValue TableModel::rowAt(int /*index*/){
    //TODO
//    return m_dataSource->rowAt(index);
    return QJSValue();
}

void TableModel::assignCell(int row, int col, QString value)
{
    setData(QAbstractItemModel::createIndex(row, col), value);
}

bool TableModel::select(QJSValue column, QJSValue row){
    if ( column.isNumber() && row.isNumber() ){
        m_rowModel->select(column.toInt(), row.toInt());
        auto index = QAbstractItemModel::createIndex(row.toInt(), column.toInt());
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool TableModel::deselect(QJSValue column, QJSValue row){
    if ( column.isUndefined() && row.isUndefined() ){
        beginResetModel();
        m_rowModel->deselectAll();
        endResetModel();
        return true;
    }
    return false;
}

void TableModel::clearTable()
{
// TODO: Modify in accordance with m_dataSource
//    for (int i =0; i<m_data.size(); ++i)
//        for (int j=0; j<m_data[i].size(); ++j)
//            m_data[i][j] = "";
//    emit dataChanged(QAbstractItemModel::createIndex(0, 0),
//                     QAbstractItemModel::createIndex(m_data.size()-1, m_data[m_data.size()-1].size()-1));
}

void TableModel::__dataSourceAboutToLoad(){
    beginResetModel();
}

void TableModel::__dataSourceFinished(){
    m_rowModel->notifyModelReset(m_dataSource->totalRows());
    m_headerModel->notifyModelReset();

    auto names = m_dataSource->columnNames();

    for ( int i = 0; i < names.size(); ++i ){
        m_headerModel->addColumn(names[i]);
    }
    endResetModel();
}

void TableModel::assignDataSource(Table *ds){
}

} // namespace
