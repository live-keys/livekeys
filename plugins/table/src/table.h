#ifndef LVTABLE_H
#define LVTABLE_H

#include <QAbstractTableModel>
#include <QQmlParserStatus>
#include <QJSValue>

namespace lv {

class TableHeader;
class TableRowsInfo;

class Table : public QAbstractTableModel, public QQmlParserStatus
{
    Q_OBJECT
    
    Q_PROPERTY(lv::TableHeader*   header        READ header         CONSTANT)
    Q_PROPERTY(lv::TableRowsInfo* rowInfo       READ rowInfo        CONSTANT)
    
    enum Roles{
        Value = Qt::UserRole + 1,
        IsSelected
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
    lv::TableRowsInfo *rowInfo() const;

signals:
    void complete();
    void rowAdded();
    void columnAdded();

public slots:
    void addRows(int number = 1);
    void addColumns(int number = 1);
    void removeColumn(int idx);
    void assignCell(int row, int col, QString value);

    bool select(QJSValue column, QJSValue row);
    bool deselect(QJSValue column = QJSValue(), QJSValue row = QJSValue());

private:

    Q_DISABLE_COPY(Table)
    QHash<int, QByteArray>            m_roles;
    QList<QList<QString>>             m_data;
    bool                              m_isComponentComplete;

    TableHeader*                      m_headerModel;
    TableRowsInfo*                    m_rowModel;
};

inline QHash<int, QByteArray> Table::roleNames() const
{
    return m_roles;
}

} // namespace

#endif // LVTABLE_H
