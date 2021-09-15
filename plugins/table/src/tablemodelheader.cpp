#include "tablemodelheader.h"
#include "tablemodel.h"
#include "tableschema.h"
#include "table.h"

namespace lv{

TableModelHeader::TableModelHeader(TableModel *parent)
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

    TableModel* tm = static_cast<TableModel*>(parent());
    if ( !tm->table()->schema() )
        return QVariant();

    return tm->table()->schema()->fieldAt(index.column())->name();
}

void TableModelHeader::notifyColumnAdded(int index)
{
    beginInsertColumns(QModelIndex(), index, index);
    auto modelData = createColumnData();
    m_data.insert(index, modelData);
    endInsertColumns();
}

void TableModelHeader::notifyColumnRemoved(int idx)
{
    beginRemoveColumns(QModelIndex(), idx, idx);

    m_contentWidth -= m_data[idx].width;
    emit contentWidthChanged();
    m_data.erase(m_data.begin()+idx);

    endRemoveColumns();
}

void TableModelHeader::notifyModelReset(){
    beginResetModel();
    m_data.clear();

    TableModel* tm = static_cast<TableModel*>(parent());
    if ( tm->table()->schema() ){
        int totalFields = tm->table()->schema()->totalFields();
        for ( int i = 0; i < totalFields; ++i ){
            auto modelData = createColumnData();
            m_data.append(modelData);
        }
    }
    endResetModel();
    m_contentWidth = m_data.size() * m_defaultColumnWidth;
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

TableModelHeader::ColumnData TableModelHeader::createColumnData(){
    TableModelHeader::ColumnData data;
    data.width = m_defaultColumnWidth;
    m_contentWidth += m_defaultColumnWidth;
    emit contentWidthChanged();
    return data;
}

int TableModelHeader::contentWidth() const
{
    return m_contentWidth;
}


}

