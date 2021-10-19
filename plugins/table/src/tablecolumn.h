#ifndef LVTABLECOLUMN_H
#define LVTABLECOLUMN_H

#include <QObject>
#include "table.h"

namespace lv{

class TableColumn : public Table{

    Q_OBJECT
    Q_PROPERTY(lv::Table* table READ table  WRITE setTable   NOTIFY tableChanged)
    Q_PROPERTY(QJSValue column  READ column WRITE setColumn NOTIFY columnChanged)

public:
    explicit TableColumn(QObject *parent = nullptr);
    ~TableColumn();

    Table* table() const;
    void setTable(lv::Table* table);

    QJSValue column() const;
    void setColumn(const QJSValue& val);

    void process();
    QString valueAt(int row, int column) override;

    void insertRow(const QJSValue& row, InsertCallback cb) override;
    void updateRow(int index, const QJSValue& row) override;
    void removeRow(int index) override;
    int totalRows() const override;
    QJSValue rowAt(int index) override;

    void insertField(int index, TableField::Ptr field) override;
    void updateField(int index, const QJSValue& opt) override;
    void removeField(int index) override;

public slots:
    void __tableRowsInserted(const QModelIndex &parent, int first, int last);
    void __tableRowsRemoved(const QModelIndex &parent, int first, int last);
    void __tableDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void __tableReset();

signals:
    void tableChanged();
    void columnChanged();

private:
    void throwTableReadOnly();

    Q_DISABLE_COPY(TableColumn);

    Table*         m_table;
    QJSValue       m_column;
    QList<QString> m_data;
};

inline Table *TableColumn::table() const{
    return m_table;
}

inline QJSValue TableColumn::column() const{
    return m_column;
}

inline void TableColumn::setColumn(const QJSValue& column){
    m_column = column;
    emit columnChanged();
    process();
}

}// namespace

#endif // LVTABLECOLUMN_H
