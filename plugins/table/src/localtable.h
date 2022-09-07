#ifndef LVLOCALTABLE_H
#define LVLOCALTABLE_H

#include <QObject>
#include <QJSValue>
#include "table.h"
#include "live/qmlmetaextension.h"

namespace lv{

class LocalTable : public Table{

    Q_OBJECT

    META_EXTENSION_BASE(LocalTable, &LocalTable::objectConstructor);

public:
    explicit LocalTable(QObject *parent = nullptr);
    ~LocalTable();

    QString valueAt(int row, int column) override;

    void insertRow(const QJSValue& row, InsertCallback cb) override;
    void updateRow(int index, const QJSValue& row) override;
    void removeRow(int index) override;
    int totalRows() const override;
    QJSValue rowAt(int index) override;

    void insertField(int index, TableField::Ptr field) override;
    void updateField(int index, const QJSValue& opt) override;
    void removeField(int index) override;

    static QObject* objectConstructor();

    static void serialize(ViewEngine* ve, const QObject* ob, MLNode& result);
    static QObject* deserialize(ViewEngine* ve, const MLNode& n);

public slots:
    void readFromFile(const QString& path, const QJSValue& options);
    void writeToFile(const QString& path, const QJSValue& options);

    void clear();

private:
    Q_DISABLE_COPY(LocalTable);

    QList<QList<QString> > m_data;
};

inline QObject *LocalTable::objectConstructor(){
    return new LocalTable;
}

}// namespace

#endif // LVLOCALTABLE_H
