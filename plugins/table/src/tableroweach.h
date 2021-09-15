#ifndef LVTABLEROWEACH_H
#define LVTABLEROWEACH_H

#include <QObject>
#include "table.h"

namespace lv{

class TableRowEach : public Table{

    Q_OBJECT
    Q_PROPERTY(lv::Table* table READ table WRITE setTable NOTIFY tableChanged)
    Q_PROPERTY(QJSValue fn      READ fn    WRITE setFn    NOTIFY fnChanged)

public:
    explicit TableRowEach(QObject *parent = nullptr);
    ~TableRowEach();

    Table* table() const;
    void setTable(lv::Table* table);

    QJSValue fn() const;
    void setFn(QJSValue fn);

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
    void fnChanged();

private:
    void throwTableReadOnly();

    Q_DISABLE_COPY(TableRowEach);

    lv::Table* m_table;
    QJSValue m_fn;
    QList<QList<QString> > m_data;
};

inline Table *TableRowEach::table() const{
    return m_table;
}

inline QJSValue TableRowEach::fn() const{
    return m_fn;
}

inline void TableRowEach::setFn(QJSValue fn){
    m_fn = fn;
    emit fnChanged();
    process();
}

}// namespace

#endif // LVTABLEROWEACH_H
