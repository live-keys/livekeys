#include "qprojectfile.h"

#include <QDebug>
namespace lcv{

QProjectFile::QProjectFile(const QString &path, QProjectEntry *parent)
    : QProjectEntry(path, parent)
    , m_isActive(false)
    , m_isOpen(false)
    , m_isDirty(false)
{
}

QProjectFile::QProjectFile(const QString &path, const QString &name, QProjectEntry *parent)
    : QProjectEntry(path, name, parent)
    , m_isActive(false)
    , m_isOpen(false)
    , m_isDirty(false)
{

}

QProjectFile::~QProjectFile(){
}

}// namespace
