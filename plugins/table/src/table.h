#ifndef LVTABLE
#define LVTABLE

#include <QObject>

namespace lv{

class Table : public QObject{

    Q_OBJECT

public:
    explicit Table(QObject *parent = nullptr);
    virtual ~Table();

    virtual int totalRows() const = 0;
    virtual QString valueAt(int row, int column) = 0;
    virtual void setValueAt(int row, int column, const QString& value) = 0;

    virtual void addRow() = 0;
    virtual void addColumn() = 0;
    virtual void removeColumn(int idx) = 0;
    virtual void removeRow(int idx) = 0;
    virtual int totalColumns() const = 0;
    virtual QList<QString> columnNames() const = 0;

    void beginLoadData();
    void endLoadData();

public slots:
    void reload();

signals:
    void dataAboutToLoad();
    void dataLoaded();
};

}// namespace

#endif // LVTABLE
