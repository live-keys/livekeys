#include "qtable.h"
#include <QDebug>

QTable::QTable(QObject *parent)
    : QAbstractTableModel(parent)
    , m_headerModel(new QTable(false))
    , m_rowModel(new QTable(false))
{
    m_headerModel->m_data = {{}};
    m_roles[Value] = "value";
    initializeData();
}

QTable::~QTable()
{

}

int QTable::rowCount(const QModelIndex &) const
{
    return m_data.size();
}

int QTable::columnCount(const QModelIndex &) const
{
    int max = 0;
    for (auto row: m_data)
        if (row.size()>max)
            max = row.size();

    return max;
}

Qt::ItemFlags QTable::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}


QVariant QTable::data(const QModelIndex &index, int) const
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

bool QTable::setData(const QModelIndex &index, const QVariant &value, int)
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
}

QTable *QTable::headerModel() const
{
    return m_headerModel;
}

QTable *QTable::rowModel() const
{
    return m_rowModel;
}

void QTable::initializeData()
{
    beginInsertRows(QModelIndex(), 0, 3);
    m_data = {
        {"a", "b", "c"},
        {"d", "e"},
        {"g", "f", "h", "i", "j", "K"},
        {"g", "f", "h", "i", "j", "K"}
    };
    endInsertRows();

    m_headerModel->beginInsertRows(QModelIndex(),0,0);
    int colNum = columnCount();
    for (int i = 0; i < colNum; ++i){
        m_headerModel->setData(QAbstractItemModel::createIndex(0, i), QTable::letterIndex(i));
    }
    m_headerModel->endInsertRows();
}

void QTable::addRow()
{
    int rowIndex = rowCount();

    beginInsertRows(QModelIndex(), rowIndex, rowIndex);
    m_data.push_back({});

    int colNum = columnCount();
    for (int i=0; i < colNum; ++i)
        setData(QAbstractItemModel::createIndex(rowIndex, i), "");
    endInsertRows();
}

void QTable::addColumn()
{
    int colIndex = columnCount();
    beginInsertColumns(QModelIndex(), colIndex, colIndex);

    int rowNum = rowCount();
    for (int i = 0; i < rowNum; ++i)
        setData(QAbstractItemModel::createIndex(i, colIndex), "");
    endInsertColumns();

    m_headerModel->beginInsertColumns(QModelIndex(),colIndex,colIndex);
    m_headerModel->setData(QAbstractItemModel::createIndex(0, colIndex), QTable::letterIndex(colIndex));
    m_headerModel->endInsertColumns();
}

void QTable::removeColumn(int idx)
{
    if (idx >= columnCount()) return;

    beginRemoveColumns(QModelIndex(),idx, idx);
    for (int i=0; i<m_data.size(); ++i){
        if (idx >= m_data[i].size()) continue;
        m_data[i].erase(m_data[i].begin()+idx);
    }
    endRemoveColumns();

    m_headerModel->beginRemoveColumns(QModelIndex(),idx,idx);
    m_headerModel->m_data[0].erase(m_headerModel->m_data[0].begin() + idx);
    m_headerModel->endRemoveColumns();
}

QString QTable::letterIndex(int idx)
{
    int rem = idx%26;
    QString value = QString(static_cast<char>(static_cast<int>('A') + rem));
    idx /= 26;

    while (idx){
        int rem = idx%26;
        idx = idx/26;
        value = QString(static_cast<char>(static_cast<int>('A') + rem - 1)) + value;
    }
    return value;
}

QTable::QTable(bool)
    : QAbstractTableModel(nullptr)
    , m_headerModel(nullptr)
    , m_rowModel(nullptr)
{
    m_roles[Value] = "value";
}
