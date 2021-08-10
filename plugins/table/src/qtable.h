#ifndef QTABLE_H
#define QTABLE_H

#include <QAbstractTableModel>

class QTable : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(QTable* headerModel READ headerModel CONSTANT)
    Q_PROPERTY(QTable* rowModel    READ rowModel    CONSTANT)
    enum Roles{
        Value = Qt::UserRole + 1
    };
public:

    explicit QTable(QObject *parent = nullptr);
    ~QTable();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QTable *headerModel() const;
    QTable *rowModel() const;

    void initializeData();
public slots:
    void addRow();
    void addColumn();
    void removeColumn(int idx);
private:
    static QString letterIndex(int idx);
    QTable(bool dummy);

    Q_DISABLE_COPY(QTable)
    QHash<int, QByteArray>            m_roles;
    std::vector<std::vector<QString>> m_data;

    QTable*                           m_headerModel;
    QTable*                           m_rowModel;
};

inline QHash<int, QByteArray> QTable::roleNames() const
{
    return m_roles;
}

#endif // QTABLE_H
