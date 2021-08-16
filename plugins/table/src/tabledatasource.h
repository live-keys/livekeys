#ifndef LVTABLEDATASOURCE_H
#define LVTABLEDATASOURCE_H

#include <QObject>

namespace lv{

class TableDataSource : public QObject{

    Q_OBJECT

public:
    explicit TableDataSource(QObject *parent = nullptr);
    virtual ~TableDataSource();

    virtual int totalRows() const = 0;
    virtual QString valueAt(int row, int column) = 0;
    virtual void setValueAt(int row, int column, const QString& value) = 0;

    virtual void addRow() = 0;
    virtual void addColumn() = 0;
    virtual void removeColumn(int idx) = 0;


    void beginLoadData();
    void endLoadData();

signals:
    void dataAboutToLoad();
    void dataLoaded();
};

}// namespace

#endif // LVTABLEDATASOURCE_H
