#include "table.h"
#include "tablemodel.h"

namespace lv{

Table::Table(QObject *parent)
    : QObject(parent)
    , m_model(nullptr)
{
}

Table::~Table(){
    delete m_model;
}

void Table::beginLoadData(){
    emit dataAboutToLoad();
}

void Table::endLoadData(){
    emit dataLoaded();
}

TableModel *Table::model(){
    if ( !m_model ){
        m_model = new TableModel(this);
    }
    return m_model;
}

void Table::reload(){
    beginLoadData();
    endLoadData();
}

}// namespace
