#include "listdir.h"
#include "live/visuallogqt.h"
#include <QDir>
#include <QDirIterator>

namespace lv{

ListDir::ListDir(QObject *parent)
    : QObject(parent)
    , m_flags(0)
{
}

ListDir::~ListDir(){
}

void ListDir::process(){
    if ( m_path.isEmpty() || !QDir(m_path).exists() )
        return;

    QStringList result;

    QDirIterator::IteratorFlags iflags;
    if ( m_flags & ListDir::Subdirectories )
        iflags = iflags | QDirIterator::Subdirectories;
    if ( m_flags & ListDir::FollowSymlinks )
        iflags = iflags | QDirIterator::FollowSymlinks;

    QDirIterator dit(m_path, iflags);
    while ( dit.hasNext() ){
        QString nxt = dit.next();
        QString name = dit.fileName();
        if ( name != "." && name != ".." )
            result.append(nxt);
    }

    m_output = result;
    emit outputChanged();
}

}// namespace
