#include "qprojectfile.h"
#include <QFileInfo>

namespace lcv{

QProjectFile::QProjectFile(const QString &path, QProjectEntry *parent)
    : QProjectEntry(QFileInfo(path).path(), QFileInfo(path).fileName(), true, parent)
    , m_isOpen(false)
    , m_isDirty(false)
{
}

QProjectFile::QProjectFile(const QString &path, const QString &name, QProjectEntry *parent)
    : QProjectEntry(path, name, true, parent)
    , m_isOpen(false)
    , m_isDirty(false)
{
}

QProjectFile::~QProjectFile(){
}

}// namespace
