#include "table.h"
#include "tablemodel.h"
#include "tableschema.h"

namespace lv{

Table::Table(QObject *parent)
    : Shared(parent)
    , m_schema(nullptr)
    , m_model(nullptr)
{
}

Table::~Table(){
    delete m_model;
}

bool Table::isModelSet() const{
    return m_model ? true : false;
}

TableModel *Table::model(){
    if ( !m_model ){
        m_model = new TableModel(this);
    }
    return m_model;
}

TableSchema *Table::schema() const{
    return m_schema;
}

void Table::setSchema(TableSchema *schema){
    if ( m_schema )
        delete m_schema;
    m_schema = schema;
}

}// namespace
