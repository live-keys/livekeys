#ifndef LVTABLECONCAT_H
#define LVTABLECONCAT_H

#include <QObject>
#include "table.h"

namespace lv{

class TableConcat : public Table{

    Q_OBJECT
    Q_PROPERTY(lv::Table* table1 READ table1 WRITE setTable1 NOTIFY table1Changed)
    Q_PROPERTY(lv::Table* table2 READ table2 WRITE setTable2 NOTIFY table2Changed)

public:
    explicit TableConcat(QObject *parent = nullptr);
    ~TableConcat();

    Table* table1() const;
    void setTable1(lv::Table* table);

    Table* table2() const;
    void setTable2(lv::Table* table);

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
    void __table1RowsInserted(const QModelIndex &parent, int first, int last);
    void __table1RowsRemoved(const QModelIndex &parent, int first, int last);
    void __table1DataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void __table1Reset();

    void __table2RowsInserted(const QModelIndex &parent, int first, int last);
    void __table2RowsRemoved(const QModelIndex &parent, int first, int last);
    void __table2DataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void __table2Reset();

signals:
    void table1Changed();
    void table2Changed();

private:
    void throwTableReadOnly();

    Q_DISABLE_COPY(TableConcat);

    Table*                 m_table1;
    Table*                 m_table2;
    QList<QList<QString> > m_data;
};

inline Table *TableConcat::table1() const{
    return m_table1;
}

inline Table *TableConcat::table2() const{
    return m_table2;
}


}// namespace

#endif // LVTABLECONCAT_H
