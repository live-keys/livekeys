#include "qmlfilestream.h"
#include "live/viewcontext.h"
#include "live/exception.h"
#include "live/viewengine.h"
#include "live/visuallogqt.h"

#include <QTimer>

namespace lv{

QmlFileStream::QmlFileStream(QObject *parent)
    : QObject(parent)
    , m_stream(nullptr)
    , m_wait(0)
{
}

QmlFileStream::~QmlFileStream(){
}

void QmlFileStream::wait(){
    ++m_wait;
}

void QmlFileStream::resume(){
    if ( m_wait > 0 ){
        --m_wait;
        next();
    }
}

void QmlFileStream::next(){
    while ( !m_text.atEnd() && !m_wait )
        m_stream->push(m_text.readLine());
}

QmlStream *QmlFileStream::lines(const QString &file){
    if ( m_stream ){
        if ( m_filePath == file )
            return m_stream;

        delete m_stream;
        m_stream = nullptr;
    }
    if ( !QFile::exists(file) ){
        Exception e = CREATE_EXCEPTION(Exception, "File does not exists: " + file.toStdString(), Exception::toCode("~File"));
        ViewContext::instance().engine()->throwError(&e, this);
        return nullptr;
    }

    m_file.setFileName(file);
    if ( !m_file.open(QIODevice::ReadOnly) ){
        Exception e = CREATE_EXCEPTION(Exception, "Failed to open file for reading: " + file.toStdString(), Exception::toCode("~File"));
        ViewContext::instance().engine()->throwError(&e, this);
        return nullptr;
    }

    m_text.setDevice(&m_file);

    m_filePath = file;
    m_stream   = new QmlStream(this, this);

    QTimer::singleShot(0, this, &QmlFileStream::next);

    return m_stream;
}

}// namespace
