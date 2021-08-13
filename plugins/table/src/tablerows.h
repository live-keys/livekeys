#ifndef TABLEROWS_H
#define TABLEROWS_H

#include <QAbstractTableModel>

namespace lv {

class TableRows : public QAbstractTableModel
{
    Q_OBJECT
    enum Roles{
        Value = Qt::UserRole + 1
    };
public:
    explicit TableRows(QObject* parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addRow();
    void initializeData(int num);
private:
    int                     m_num;
    QHash<int, QByteArray>  m_roles;
};

inline QHash<int, QByteArray> TableRows::roleNames() const
{
    return m_roles;
}

}

#endif // TABLEROWS_H
