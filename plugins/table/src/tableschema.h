#ifndef LVTABLESCHEMA_H
#define LVTABLESCHEMA_H

#include <QList>
#include "tablefield.h"

namespace lv{

class TableSchema{

public:
    TableSchema();

    void insertField(int index, TableField::Ptr field);
    void removeField(int index);
    void clearFields();
    int totalFields() const;
    const TableField::Ptr fieldAt(int index) const;
    int fieldIndex(const QString& name) const;

private:
    QList<TableField::Ptr> m_fields;
};

}// namespace

#endif // LVTABLESCHEMA_H
