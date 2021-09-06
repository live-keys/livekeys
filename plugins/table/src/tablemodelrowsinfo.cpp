#include "tablemodelrowsinfo.h"
#include "tablemodel.h"
#include <QDebug>

namespace lv {

TableModelRowsInfo::TableModelRowsInfo(QObject* parent)
    : QAbstractTableModel(parent)
    , m_rowCount(0)
    , m_defaultRowHeight(25)
    , m_contentHeight(0)
{
    m_roles[Value] = "value";
}

int TableModelRowsInfo::rowCount(const QModelIndex &) const
{
    return m_rowCount;
}

int TableModelRowsInfo::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant TableModelRowsInfo::data(const QModelIndex &index, int) const{
    if (index.column() > 0)
        return QVariant();
    if (index.row() >= m_rowCount)
        return QVariant();

    return QString::number(index.row()+1);
}

void TableModelRowsInfo::notifyRowAdded()
{
    beginInsertRows(QModelIndex(), m_rowCount, m_rowCount);
    ++m_rowCount;
    m_contentHeight += m_defaultRowHeight;
    emit contentHeightChanged();
    endInsertRows();
}

void TableModelRowsInfo::notifyColumnAdded(){
    for ( auto it = m_data.begin(); it != m_data.end(); ++it ){
        TableModelRowsInfo::CellInfo ci;
        ci.isSelected = false;
        it.value()->cells.append(ci);
    }
}

void TableModelRowsInfo::notifyModelReset(int newRowCount){
    beginResetModel();
    for ( auto it = m_data.begin(); it != m_data.end(); ++it ){
        delete it.value();
    }
    m_data.clear();
    m_rowCount = newRowCount;
    m_contentHeight = newRowCount * m_defaultRowHeight;
    endResetModel();
    emit contentHeightChanged();
}

void TableModelRowsInfo::initializeData(int num)
{
    beginInsertRows(QModelIndex(), 0, num-1);
    m_rowCount = num;
    m_contentHeight = num*m_defaultRowHeight;
    emit contentHeightChanged();
    endInsertRows();
}

void TableModelRowsInfo::removeRow(int idx)
{
    beginRemoveRows(QModelIndex(), idx, idx);
    m_contentHeight -= rowHeight(idx);
    m_data.remove(idx);
    QList<int> keys = m_data.keys();
    QMap<int, RowData*> copy;
    for (auto key: keys){
        if (key < idx)
            copy[key] = m_data[key];
        else
            copy[key-1] = m_data[key];
    }

    m_data.swap(copy);
    --m_rowCount;

    endRemoveRows();
}

void TableModelRowsInfo::removeColumn(int idx)
{
    for ( auto it = m_data.begin(); it != m_data.end(); ++it ){
        auto rowInfo = *it;
        rowInfo->cells.erase(rowInfo->cells.begin() + idx);
    }
}

QString TableModelRowsInfo::toString() const{
    QString result;
    for ( auto it = m_data.begin(); it != m_data.end(); ++it ){
        result += it.key() + QString(":") + "height" + it.value()->height + "\n";

        for ( int i = 0; i < it.value()->cells.size(); ++i ){
            if ( it.value()->cells[i].isSelected ){
                result += "  Selected: " + QString::number(i);
            }
        }
    }
    return result;
}

void TableModelRowsInfo::updateRowHeight(int index, int height){
    if (height == rowHeight(index))
        return;

    auto rowData = rowDataAtWithCreate(index);
    int delta = height - rowData->height;
    rowData->height = height;
    m_contentHeight += delta;
    emit contentHeightChanged();

}

int TableModelRowsInfo::rowHeight(int index) const{
    auto rowData = rowDataAt(index);
    if ( rowData )
        return rowData->height;
    return m_defaultRowHeight;
}

TableModelRowsInfo::RowData *TableModelRowsInfo::rowDataAtWithCreate(int index){
    auto it = m_data.find(index);
    if ( it != m_data.end() ){
        return it.value();
    } else {
        TableModelRowsInfo::RowData* data = new TableModelRowsInfo::RowData;
        data->height = m_defaultRowHeight;

        TableModel* table = qobject_cast<TableModel*>(parent());
        int cols = table->columnCount();
        for ( int i = 0; i < cols; ++i ){
            TableModelRowsInfo::CellInfo ci;
            ci.isSelected = false;
            data->cells.append(ci);
        }
        m_data.insert(index, data);
        return data;
    }
}

TableModelRowsInfo::RowData *TableModelRowsInfo::rowDataAt(int index) const{
    auto it = m_data.find(index);
    if ( it != m_data.end() )
        return it.value();
    return nullptr;
}

int TableModelRowsInfo::contentHeight() const
{
    return m_contentHeight;
}

bool TableModelRowsInfo::isSelected(int column, int row) const{
    TableModelRowsInfo::RowData* rd = rowDataAt(row);
    if ( !rd )
        return false;

    return rd->cells[column].isSelected;
}

void TableModelRowsInfo::select(int column, int row){
    TableModelRowsInfo::RowData* rd = rowDataAtWithCreate(row);
    rd->cells[column].isSelected = true;
}

void TableModelRowsInfo::deselectAll(){
    for ( auto it = m_data.begin(); it != m_data.end(); ++it ){
        for ( int i = 0; i < it.value()->cells.size(); ++i ){
            if ( it.value()->cells[i].isSelected ){
                it.value()->cells[i].isSelected = false;
            }
        }
    }
}

}
