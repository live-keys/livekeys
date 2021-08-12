#ifndef LVTABLE_H
#define LVTABLE_H

#include <QAbstractTableModel>
#include <QQmlParserStatus>

namespace lv {

class TableHeader;
class TableRows;

class Table : public QAbstractTableModel, public QQmlParserStatus
{
    Q_OBJECT
    
    Q_PROPERTY(lv::TableHeader* header   READ header   CONSTANT)
    Q_PROPERTY(lv::TableRows*   rowModel READ rowModel CONSTANT)
    
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

    void classBegin() override {}
    virtual void componentComplete() override;

    lv::TableHeader *header() const;
    lv::TableRows *rowModel() const;

signals:
    void complete();
    void rowAdded();
    void columnAdded();
public slots:
    void addRow();
    void addColumn();
    void removeColumn(int idx);
    void assignCell(int row, int col, QString value);
private:

    Q_DISABLE_COPY(Table)
    QHash<int, QByteArray>            m_roles;
    std::vector<std::vector<QString>> m_data;
    bool                              m_isComponentComplete;

    TableHeader*                      m_headerModel;
    TableRows*                        m_rowModel;
};

inline QHash<int, QByteArray> Table::roleNames() const
{
    return m_roles;
}

} // namespace

#endif // LVTABLE_H
