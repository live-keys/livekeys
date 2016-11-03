#include "qprojectfile.h"

namespace lcv{

QProjectFile::QProjectFile(const QString &path, QProjectEntry *parent)
    : QProjectEntry(path, parent)
{
}

QProjectFile::QProjectFile(const QString &path, const QString &name, QProjectEntry *parent)
    : QProjectEntry(path, name, parent){
}

QProjectFile::~QProjectFile(){
}

}// namespace
