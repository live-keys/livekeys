#include "tableschema.h"

namespace  lv{

TableSchema::TableSchema()
{
}

void TableSchema::insertField(int index, TableField::Ptr field){
    if ( index == -1 )
        index = m_fields.size();
    m_fields.insert(index, field);
}

void TableSchema::removeField(int){
    //TODO
}

void TableSchema::clearFields(){
    m_fields.clear();
}

int TableSchema::totalFields() const{
    return m_fields.size();
}

const TableField::Ptr TableSchema::fieldAt(int index) const{
    return m_fields.at(index);
}

int TableSchema::fieldIndex(const QString &name) const{
    for ( int i = 0; i < m_fields.length(); ++i ){
        if ( m_fields.at(i)->name() == name )
            return i;
    }
    return -1;
}

}// namespace
