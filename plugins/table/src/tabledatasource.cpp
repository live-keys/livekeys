#include "tabledatasource.h"

namespace lv{

TableDataSource::TableDataSource(QObject *parent)
    : QObject(parent)
{
}

TableDataSource::~TableDataSource(){
}

void TableDataSource::beginLoadData(){
    emit dataAboutToLoad();
}

void TableDataSource::endLoadData(){
    emit dataLoaded();
}

}// namespace
