#include "table.h"

namespace lv{

Table::Table(QObject *parent)
    : QObject(parent)
{
}

Table::~Table(){
}

void Table::beginLoadData(){
    emit dataAboutToLoad();
}

void Table::endLoadData(){
    emit dataLoaded();
}

void Table::reload(){
    beginLoadData();
    endLoadData();
}

}// namespace
