#ifndef LVTABLETOLIST_H
#define LVTABLETOLIST_H

#include <QObject>
#include "table.h"

namespace lv{

class TableToList : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::Table* table READ table WRITE setTable NOTIFY tableChanged)
    Q_PROPERTY(QJSValue result  READ result NOTIFY resultChanged)

public:
    explicit TableToList(QObject *parent = nullptr);
    ~TableToList();

    lv::Table* table() const;
    void setTable(lv::Table* table);

    const QJSValue &result() const;

    void process();

public slots:
    void __tableRowsInserted(const QModelIndex &parent, int first, int last);
    void __tableRowsRemoved(const QModelIndex &parent, int first, int last);
    void __tableDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void __tableReset();

signals:
    void tableChanged();
    void resultChanged();

private:
    lv::Table* m_table;
    QJSValue   m_result;
};

inline Table *TableToList::table() const{
    return m_table;
}

inline const QJSValue& TableToList::result() const{
    return m_result;
}

}// namespace

#endif // LVTABLETOLIST_H
