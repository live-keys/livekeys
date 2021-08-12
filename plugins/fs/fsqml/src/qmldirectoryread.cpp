#include "qmldirectoryread.h"
#include "live/visuallogqt.h"
#include "live/viewengine.h"

namespace lv{

QmlDirectoryRead::QmlDirectoryRead(QObject *parent)
    : QObject(parent)
    , m_stream(nullptr)
    , m_wait(0)
    , m_flags(0)
    , m_dit(nullptr)
{
}

QmlDirectoryRead::~QmlDirectoryRead(){
    if ( m_stream )
        Shared::unref(m_stream);
    delete m_dit;
}

void QmlDirectoryRead::wait(){
    ++m_wait;
}

void QmlDirectoryRead::resume(){
    if ( m_wait > 0 )
        --m_wait;
    if ( m_wait == 0 ){
        next();
    }
}

void QmlDirectoryRead::next(){
    while ( m_dit && m_dit->hasNext() && !m_wait ){
        QString nxt = m_dit->next();
        QString name = m_dit->fileName();
        if ( name != "." && name != ".." ){
            m_stream->push(nxt);
        }
    }
}

void QmlDirectoryRead::setPath(const QString &path){
    if (m_path == path)
        return;

    m_path = path;
    emit pathChanged();
}

void QmlDirectoryRead::start(){
    ViewEngine* engine = ViewEngine::grab(this);

    if ( !QDir(m_path).exists() ){
        Exception e = CREATE_EXCEPTION(Exception, "Path does not exists: " + m_path.toStdString(), Exception::toCode("~Directory"));
        engine->throwError(&e, this);
        return;
    }

    if ( m_stream ){
        Shared::unref(m_stream);
        m_stream = nullptr;
    }
    if ( m_dit ){
        delete m_dit;
        m_dit = nullptr;
    }

    m_stream = new QmlStream(this, this);
    Shared::ref(m_stream);
    emit streamChanged();

    QDirIterator::IteratorFlags iflags;
    if ( m_flags & QmlDirectoryRead::Subdirectories )
        iflags = iflags | QDirIterator::Subdirectories;
    if ( m_flags & QmlDirectoryRead::FollowSymlinks )
        iflags = iflags | QDirIterator::FollowSymlinks;

    m_dit = new QDirIterator(m_path, iflags);
    next();
}

void QmlDirectoryRead::stop(){
    m_wait = true;
}

}// namespace
