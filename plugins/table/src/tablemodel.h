#ifndef LVTABLEMODEL_H
#define LVTABLEMODEL_H

#include <QAbstractTableModel>
#include <QQmlParserStatus>
#include <QJSValue>

namespace lv {

class TableModelHeader;
class TableModelRowsInfo;
class Table;

class TableModel : public QAbstractTableModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(lv::TableModelHeader*   header   READ header  CONSTANT)
    Q_PROPERTY(lv::TableModelRowsInfo* rowInfo  READ rowInfo CONSTANT)
    Q_PROPERTY(lv::Table* table                 READ table   CONSTANT)
    
    enum Roles{
        Value = Qt::UserRole + 1,
        IsSelected
    };
public:

    explicit TableModel(Table *parent = nullptr);
    ~TableModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    void classBegin() override {}
    virtual void componentComplete() override;

    lv::TableModelHeader *header() const;
    lv::TableModelRowsInfo *rowInfo() const;
    lv::Table* table() const;

    void setTable(lv::Table* table);

signals:
    void complete();
    void rowAdded();
    void columnAdded();

public slots:
    void addRows(int number = 1);
    void removeRow(int idx);
    void assignCell(int row, int col, QString value);

    void addColumns(int number = 1);
    void assignColumnInfo(int index, QJSValue name);
    void removeColumn(int idx);

    bool select(QJSValue column, QJSValue row);
    bool deselect(QJSValue column = QJSValue(), QJSValue row = QJSValue());

    void clearTable();
    void __dataSourceAboutToLoad();
    void __dataSourceFinished();

private:
    void assignDataSource(Table* ds);

    Q_DISABLE_COPY(TableModel)
    QHash<int, QByteArray>            m_roles;
    bool                              m_isComponentComplete;

    TableModelHeader*                      m_headerModel;
    TableModelRowsInfo*                    m_rowModel;
    Table*                  m_dataSource;
};

inline QHash<int, QByteArray> TableModel::roleNames() const{
    return m_roles;
}

inline Table *TableModel::table() const{
    return m_dataSource;
}

} // namespace

#endif // LVTABLEMODEL_H
