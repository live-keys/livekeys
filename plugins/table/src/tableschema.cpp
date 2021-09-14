#include "tableschema.h"

namespace  lv{

TableSchema::TableSchema()
{
}

void TableSchema::insertField(int index, TableField::Ptr field){
    m_fields.insert(index, field);
}

void TableSchema::removeField(int){
    //TODO
}

int TableSchema::totalFields() const{
    return m_fields.size();
}

const TableField::Ptr TableSchema::fieldAt(int index) const{
    return m_fields.at(index);
}

}// namespace
