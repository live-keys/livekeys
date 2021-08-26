#ifndef LVLOCALDATASOURCE_H
#define LVLOCALDATASOURCE_H

#include <QObject>
#include <QJSValue>
#include "tabledatasource.h"

namespace lv{

class LocalDataSource : public TableDataSource{

    Q_OBJECT

public:
    explicit LocalDataSource(QObject *parent = nullptr);
    ~LocalDataSource();

    QString valueAt(int row, int column) override;
    void setValueAt(int row, int column, const QString& value) override;

    void addRow() override;
    void addColumn() override;
    void removeColumn(int idx) override;
    void removeRow(int idx) override;
    int totalColumns() const override;
    QList<QString> columnNames() const override;

public slots:
    void readFromFile(const QString& path, const QJSValue& options);
    void writeToFile(const QString& path);

    int totalRows() const override;
    QJSValue rowAt(int index);
    QJSValue columnInfo() const;
    void clear();

private:
    Q_DISABLE_COPY(LocalDataSource);

    QList<QString>         m_headers;
    QList<QList<QString> > m_data;
};

}// namespace

#endif // LVLOCALDATASOURCE_H
