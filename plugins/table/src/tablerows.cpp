#include "tablerows.h"
#include <QDebug>

namespace lv {

TableRows::TableRows(QObject* parent)
    : QAbstractTableModel(parent)
    , m_num(0)
{
    m_roles[Value] = "value";
}

int TableRows::rowCount(const QModelIndex &) const
{
    return m_num;
}

int TableRows::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant TableRows::data(const QModelIndex &index, int) const
{
    if (index.column() > 0) return QVariant();
    if (index.row() >= m_num) return QVariant();

    return QString::number(index.row()+1);
}

void TableRows::addRow()
{
    qDebug() << "=========== rowModel add row";
    beginInsertRows(QModelIndex(), m_num, m_num);
    ++m_num;
    endInsertRows();
}

void TableRows::initializeData(int num)
{
    beginInsertRows(QModelIndex(), 0, num-1);
    m_num = num;
    endInsertRows();
}

}
