#ifndef LVLOCALTABLE_H
#define LVLOCALTABLE_H

#include <QObject>
#include <QJSValue>
#include "table.h"

namespace lv{

class LocalTable : public Table{

    Q_OBJECT

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

public slots:
    void readFromFile(const QString& path, const QJSValue& options);
    void writeToFile(const QString& path, const QJSValue& options);

    void clear();

private:
    Q_DISABLE_COPY(LocalTable);

    QList<QList<QString> > m_data;
};

}// namespace

#endif // LVLOCALTABLE_H
