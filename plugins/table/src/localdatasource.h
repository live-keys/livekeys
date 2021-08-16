#ifndef LVLOCALDATASOURCE_H
#define LVLOCALDATASOURCE_H

#include <QObject>
#include "tabledatasource.h"

namespace lv{

class LocalDataSource : public TableDataSource{

    Q_OBJECT

public:
    explicit LocalDataSource(QObject *parent = nullptr);
    ~LocalDataSource();

    int totalRows() const override;
    QString valueAt(int row, int column) override;
    void setValueAt(int row, int column, const QString& value) override;

    void addRow() override;
    void addColumn() override;
    void removeColumn(int idx) override;

public slots:
    void readFromFile(const QString& path);
    void writeToFile(const QString& path);

private:
    Q_DISABLE_COPY(LocalDataSource);

    QList<QString>         m_headers;
    QList<QList<QString> > m_data;
};

}// namespace

#endif // LVLOCALDATASOURCE_H
