#include "qmldirectorytostream.h"
#include "live/visuallogqt.h"
#include "live/viewengine.h"
#include "qmldir.h"

namespace lv{

QmlDirectoryToStream::QmlDirectoryToStream(ViewEngine *ve, const QString &path, int flags, QObject *parent)
    : QObject(parent)
    , m_engine(ve)
    , m_path(path)
    , m_stream(new QmlStream(this, this))
    , m_wait(0)
    , m_flags(flags)
    , m_dit(nullptr)
{
    Shared::ref(m_stream);

    QDirIterator::IteratorFlags iflags;
    if ( m_flags & QmlDir::Subdirectories )
        iflags = iflags | QDirIterator::Subdirectories;
    if ( m_flags & QmlDir::FollowSymlinks )
        iflags = iflags | QDirIterator::FollowSymlinks;

    m_dit = new QDirIterator(m_path, iflags);
}

QmlDirectoryToStream::~QmlDirectoryToStream(){
    Shared::unref(m_stream);
    delete m_dit;
}

void QmlDirectoryToStream::wait(){
    ++m_wait;
}

void QmlDirectoryToStream::resume(){
    if ( m_wait > 0 )
        --m_wait;
    if ( m_wait == 0 ){
        next();
    }
}

void QmlDirectoryToStream::next(){
    while ( m_dit && m_dit->hasNext() && !m_wait ){
        QString nxt = m_dit->next();
        QString name = m_dit->fileName();
        if ( name != "." && name != ".." ){
            m_stream->push(nxt);
        }
    }
}


}// namespace
