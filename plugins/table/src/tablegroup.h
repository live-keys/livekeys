#ifndef LVTABLEGROUP_H
#define LVTABLEGROUP_H

#include <QObject>
#include <QQmlParserStatus>
#include "table.h"

namespace lv{

class TableGroup :  public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(lv::Table* table     READ table      WRITE setTable      NOTIFY tableChanged)
    Q_PROPERTY(QJSValue all         READ all        WRITE setAll        NOTIFY allChanged)
    Q_PROPERTY(QJSValue reduce      READ reduce     WRITE setReduce     NOTIFY reduceChanged)
    Q_PROPERTY(QJSValue reduceInit  READ reduceInit WRITE setReduceInit NOTIFY reduceInitChanged)
    Q_PROPERTY(QJSValue result      READ result     NOTIFY resultChanged)

public:
    explicit TableGroup(QObject *parent = nullptr);
    ~TableGroup();

    Table* table() const;
    void setTable(lv::Table* table);

    QJSValue all() const;
    void setAll(QJSValue fn);

    const QJSValue& reduce() const;
    void setReduce(QJSValue reduce);

    const QJSValue& reduceInit() const;
    void setReduceInit(QJSValue reduceInit);

    void process();

    const QJSValue& result() const;

protected:
    void componentComplete();
    void classBegin(){}

public slots:
    void __tableReset();

signals:
    void tableChanged();
    void allChanged();
    void resultChanged();
    void reduceChanged();
    void reduceInitChanged();

private:
    Q_DISABLE_COPY(TableGroup);

    QJSValue getRow(int index) const;

    lv::Table* m_table;
    bool     m_isComponentComplete;
    QJSValue m_reduce;
    QJSValue m_all;
    QJSValue m_rowExtras;
    QJSValue m_result;
    QJSValue m_reduceInit;
};

inline Table *TableGroup::table() const{
    return m_table;
}

inline QJSValue TableGroup::all() const{
    return m_all;
}

inline void TableGroup::setAll(QJSValue fn){
    m_all = fn;
    emit allChanged();
    process();
}

inline const QJSValue &TableGroup::result() const{
    return m_result;
}

inline const QJSValue &TableGroup::reduceInit() const{
    return m_reduceInit;
}

inline void TableGroup::setReduceInit(QJSValue reduceInit){
    m_reduceInit = reduceInit;
    emit reduceInitChanged();
    process();
}

inline const QJSValue &TableGroup::reduce() const{
    return m_reduce;
}

inline void TableGroup::setReduce(QJSValue reduce){
    m_reduce = reduce;
    emit reduceChanged();
    process();
}

}// namespace

#endif // LVTABLEGROUP_H
