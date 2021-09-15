#ifndef LVTABLEGROUP_H
#define LVTABLEGROUP_H

#include <QObject>
#include "table.h"

namespace lv{

class TableGroup :  public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::Table* table READ table  WRITE setTable NOTIFY tableChanged)
    Q_PROPERTY(QJSValue fn      READ fn     WRITE setFn    NOTIFY fnChanged)
    Q_PROPERTY(QJSValue result  READ result WRITE setFn    NOTIFY resultChanged)


public:
    explicit TableGroup(QObject *parent = nullptr);
    ~TableGroup();

    Table* table() const;
    void setTable(lv::Table* table);

    QJSValue fn() const;
    void setFn(QJSValue fn);

    void process();

    const QJSValue& result() const;

public slots:
    void __tableReset();

signals:
    void tableChanged();
    void fnChanged();
    void resultChanged();

private:
    Q_DISABLE_COPY(TableGroup);

    lv::Table* m_table;
    QJSValue m_fn;
    QJSValue m_result;
};

inline Table *TableGroup::table() const{
    return m_table;
}

inline QJSValue TableGroup::fn() const{
    return m_fn;
}

inline void TableGroup::setFn(QJSValue fn){
    m_fn = fn;
    emit fnChanged();
    process();
}

inline const QJSValue &TableGroup::result() const{
    return m_result;
}

}// namespace

#endif // LVTABLEGROUP_H
