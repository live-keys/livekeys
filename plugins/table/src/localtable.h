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
    void setValueAt(int row, int column, const QString& value) override;

    void addRow() override;
    void addColumn() override;
    void removeColumn(int idx) override;
    void removeRow(int idx) override;
    int totalColumns() const override;
    QList<QString> columnNames() const override;
    void assignColumnInfo(int index, const QString& info) override;

public slots:
    void readFromFile(const QString& path, const QJSValue& options);
    void writeToFile(const QString& path, const QJSValue& options);

    int totalRows() const override;
    QJSValue rowAt(int index);
    QJSValue columnInfo() const;
    void clear();

private:
    Q_DISABLE_COPY(LocalTable);

    QList<QString>         m_headers;
    QList<QList<QString> > m_data;
};

}// namespace

#endif // LVLOCALTABLE_H
