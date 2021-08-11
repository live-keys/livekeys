#include "tableheader.h"

namespace lv {


TableHeader::TableHeader(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roles[Value] = "value";
}

int TableHeader::rowCount(const QModelIndex &) const
{
    return 1;
}

int TableHeader::columnCount(const QModelIndex &parent) const
{
    return m_data.size();
}

QVariant TableHeader::data(const QModelIndex &index, int) const
{
    if (index.row() > 0) return QVariant("");
    if (index.column() >= m_data.size()) return QVariant("");
    return m_data[index.column()];
}

bool TableHeader::setData(const QModelIndex &index, const QVariant &, int)
{
    if (index.row() > 0) return false;

    if (index.column() >= m_data.size()){
        beginInsertColumns(QModelIndex(), m_data.size(), index.column());
        for (int i=m_data.size(); i <= index.column(); ++i)
            m_data.push_back(letterIndex(i));
        endInsertColumns();
        return true;
    }

    m_data[index.column()] = letterIndex(index.column());
}

void TableHeader::addColumn()
{
    beginInsertColumns(QModelIndex(), m_data.size(), m_data.size());
    m_data.push_back(letterIndex(m_data.size()));
    endInsertColumns();

}

void TableHeader::removeColumn()
{
    beginRemoveColumns(QModelIndex(), m_data.size()-1, m_data.size()-1);
    m_data.pop_back();
    endRemoveColumns();
}

void TableHeader::initalizeData(int size)
{
    if (size < 1) return;
    beginInsertColumns(QModelIndex(), 0, size-1);
    for (int i =0; i < size; ++i)
        m_data.push_back(letterIndex(i));
    endInsertColumns();
}

QString TableHeader::letterIndex(int idx)
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


}

