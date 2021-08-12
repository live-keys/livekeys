#ifndef LVTABLE_H
#define LVTABLE_H

#include <QAbstractTableModel>
#include "tableheader.h"

namespace lv {

class Table : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(TableHeader* header READ header CONSTANT)

    // Q_PROPERTY(Table* rowModel    READ rowModel    CONSTANT)
    enum Roles{
        Value = Qt::UserRole + 1
    };
public:

    explicit Table(QObject *parent = nullptr);
    ~Table();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    TableHeader *header() const;
    // Table *rowModel() const;

    void initializeData();
public slots:
    void addRow();
    void addColumn();
    void removeColumn(int idx);
private:

    Q_DISABLE_COPY(Table)
    QHash<int, QByteArray>            m_roles;
    std::vector<std::vector<QString>> m_data;

    TableHeader*                     m_headerModel;
    // Table*                           m_rowModel;
};

inline QHash<int, QByteArray> Table::roleNames() const
{
    return m_roles;
}

} // namespace

#endif // LVTABLE_H
