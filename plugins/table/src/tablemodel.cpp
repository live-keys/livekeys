#include "tablemodel.h"
#include "tablemodelheader.h"
#include "tablemodelrowsinfo.h"
#include "localtable.h"
#include "tableschema.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/visuallogqt.h"

namespace lv {

TableModel::TableModel(Table *parent)
    : QAbstractTableModel(parent)
    , m_isComponentComplete(false)
    , m_headerModel(new TableModelHeader(this))
    , m_rowModel(new TableModelRowsInfo(this))
    , m_table(nullptr)
{
    m_roles[Value]      = "value";
    m_roles[IsSelected] = "isSelected";

    m_table = parent;

    m_headerModel->notifyModelReset();
}

TableModel::~TableModel(){
    delete m_headerModel;
    delete m_rowModel;
}

int TableModel::rowCount(const QModelIndex &) const{
    return m_table->totalRows();
}

int TableModel::columnCount(const QModelIndex &) const{
    return m_headerModel->size();
}

Qt::ItemFlags TableModel::flags(const QModelIndex &) const{
    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant TableModel::data(const QModelIndex &index, int role) const{
    if ( index.row() >= m_table->totalRows() || index.column() >= columnCount() ){
        return QVariant();
    }

    if ( role == TableModel::Value ){
        return m_table->valueAt(index.row(), index.column());
    } else if ( role == TableModel::IsSelected){
        return m_rowModel->isSelected(index.column(), index.row());
    }
    return QVariant();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int)
{
//    if ( index.row() < m_table->totalRows() && index.column() < columnCount() ){

//        m_table->setValueAt(index.row(), index.column(), value.toString());

//        emit dataChanged(index, index);

//        return true;
//    }
    return false;
}

void TableModel::componentComplete(){
    m_isComponentComplete = true;
    emit complete();
}

TableModelHeader *TableModel::header() const{
    return m_headerModel;
}

TableModelRowsInfo *TableModel::rowInfo() const{
    return m_rowModel;
}

void TableModel::notifyTableRefresh(){
    beginResetModel();
}

void TableModel::notifyTableRefreshEnd(){
    m_headerModel->notifyModelReset();
    m_rowModel->notifyModelReset(m_table->totalRows());
    endResetModel();
}

void TableModel::insertRow(const QJSValue &row){
    m_table->insertRow(row, [this](int index){
        beginInsertRows(QModelIndex(), index, index);
        m_rowModel->notifyRowAdded(index);
        endInsertRows();
    });
}

void TableModel::updateRow(int index, const QJSValue &row){
    if ( !m_table->schema() )
        return;

    m_table->updateRow(index, row);
    emit dataChanged(createIndex(index, 0), createIndex(index, m_table->schema()->totalFields()));
}

void TableModel::removeField(int idx)
{
    if (idx >= columnCount())
        return;

    beginRemoveColumns(QModelIndex(),idx, idx);
    m_table->removeField(idx);
    m_headerModel->notifyColumnRemoved(idx);
    m_rowModel->removeColumn(idx);

    endRemoveColumns();
}

void TableModel::removeRow(int index)
{
    if (index >= rowCount())
        return;

    beginRemoveRows(QModelIndex(), index, index);
    m_table->removeRow(index);
    m_rowModel->removeRow(index);

    endRemoveRows();
}

int TableModel::totalRows() const{
    return m_table->totalRows();
}

QJSValue TableModel::rowAt(int index){
    return m_table->rowAt(index);
}

int TableModel::totalFields() const{
    if ( !m_table->schema() )
        return 0;

    return m_table->schema()->totalFields();
}

void TableModel::insertField(int index, const QJSValue &field){
    if ( !m_table->schema() )
        return;

    if ( index == -1 || index >= columnCount() )
        index = m_table->schema()->totalFields();

    beginInsertColumns(QModelIndex(), index, index);

    QString fieldName = field.hasOwnProperty("name") ? field.property("name").toString() : "";
    if ( fieldName.isEmpty() ){
        ViewEngine* ve = ViewEngine::grab(this);
        if ( !ve )
            ve = ViewEngine::grab(m_table);
        if ( !ve ){
            qWarning("TableModel: Failed to grab view engine. Errors will not report.");
            return;
        }
        Exception e = CREATE_EXCEPTION(lv::Exception, "TableModel: Field name is required when inserting field.", Exception::toCode("~Missing"));
        ve->throwError(&e, this);
        return;
    }

    TableField::Ptr tf = TableField::create(fieldName);
    m_table->insertField(index, tf);
    m_headerModel->notifyColumnAdded(index);
    m_rowModel->notifyColumnAdded(index);
    endInsertColumns();

}

void TableModel::updateField(int index, const QJSValue &info){
    //TODO: header->notifyColumnUpdated(...)
    //TODO: table->updateField(...)
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

} // namespace
