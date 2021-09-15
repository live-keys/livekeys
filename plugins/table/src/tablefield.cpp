#include "tablefield.h"

namespace lv{

TableField::Ptr TableField::create(const QString &name){
    return TableField::Ptr(new TableField(name));
}

TableField::TableField(const QString& name)
    : m_name(name)
{

}

}// namespace
