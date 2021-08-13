#include "tablerowsinfo.h"
#include <QDebug>

namespace lv {

TableRowsInfo::TableRowsInfo(QObject* parent)
    : QAbstractTableModel(parent)
    , m_num(0)
    , m_defaultRowHeight(25)
    , m_contentHeight(0)
{
    m_roles[Value] = "value";
}

int TableRowsInfo::rowCount(const QModelIndex &) const
{
    return m_num;
}

int TableRowsInfo::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant TableRowsInfo::data(const QModelIndex &index, int) const{
    if (index.column() > 0)
        return QVariant();
    if (index.row() >= m_num)
        return QVariant();

    return QString::number(index.row()+1);
}

void TableRowsInfo::addRow()
{
    beginInsertRows(QModelIndex(), m_num, m_num);
    ++m_num;
    rowDataAtWithCreate(m_num-1);
    endInsertRows();
}

void TableRowsInfo::initializeData(int num)
{
    beginInsertRows(QModelIndex(), 0, num-1);
    for (int i = 0; i < num; ++i)
        rowDataAtWithCreate(i);
    m_num = num;
    endInsertRows();
}

QString TableRowsInfo::toString() const{
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

void TableRowsInfo::updateRowHeight(int index, int height){
    auto rowData = rowDataAtWithCreate(index);
    int delta = height - rowData->height;
    m_contentHeight += delta;
    rowData->height = height;
    if (delta != 0)
        emit contentHeightChanged();
}

int TableRowsInfo::rowHeight(int index) const{
    auto rowData = rowDataAt(index);
    if ( rowData )
        return rowData->height;
    return m_defaultRowHeight;
}

TableRowsInfo::RowData *TableRowsInfo::rowDataAtWithCreate(int index){
    auto it = m_data.find(index);
    if ( it != m_data.end() ){
        return it.value();
    } else {
        TableRowsInfo::RowData* data = new TableRowsInfo::RowData;
        data->height = m_defaultRowHeight;
        m_contentHeight += m_defaultRowHeight;
        emit contentHeightChanged();
        m_data.insert(index, data);
        return data;
    }
}

TableRowsInfo::RowData *TableRowsInfo::rowDataAt(int index) const{
    auto it = m_data.find(index);
    if ( it != m_data.end() )
        return it.value();
    return nullptr;
}

int TableRowsInfo::contentHeight() const
{
    return m_contentHeight;
}

}
