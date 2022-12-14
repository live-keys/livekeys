#ifndef LVTABLE
#define LVTABLE

#include <QObject>
#include <QJSValue>

#include "live/shared.h"
#include "tablefield.h"

namespace lv{

class TableModel;
class TableSchema;
class Table : public Shared{

    Q_OBJECT
    Q_PROPERTY(lv::TableModel* model READ model CONSTANT)

public:
    typedef std::function<void(int)> InsertCallback;

public:
    explicit Table(QObject *parent = nullptr);
    virtual ~Table();

    virtual QString valueAt(int row, int column) = 0;
    virtual void insertRow(const QJSValue& row, InsertCallback cb) = 0;
    virtual void updateRow(int index, const QJSValue& row) = 0;
    virtual void removeRow(int index) = 0;
    virtual int totalRows() const = 0;
    virtual QJSValue rowAt(int index) = 0;

    virtual void insertField(int index, TableField::Ptr field) = 0;
    virtual void updateField(int index, const QJSValue& opt) = 0;
    virtual void removeField(int index) = 0;

    bool isModelSet() const;
    TableModel*  model();
    TableSchema* schema() const;

protected:
    void setSchema(TableSchema* schema);

private:
    TableSchema* m_schema;
    TableModel*  m_model;
};

}// namespace

#endif // LVTABLE
