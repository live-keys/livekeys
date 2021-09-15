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
    int totalFields() const;
    const TableField::Ptr fieldAt(int index) const;

private:
    QList<TableField::Ptr> m_fields;
};

}// namespace

#endif // LVTABLESCHEMA_H
