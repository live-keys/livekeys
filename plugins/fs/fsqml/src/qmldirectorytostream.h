#ifndef LVQMLDIRECTORYTOSTREAM_H
#define LVQMLDIRECTORYTOSTREAM_H

#include <QObject>
#include <QDir>
#include <QDirIterator>

#include "live/qmlstream.h"
#include "live/qmlstreamprovider.h"

namespace lv{

class QmlDirectoryToStream : public QObject, public QmlStreamProvider{

    Q_OBJECT

public:
    explicit QmlDirectoryToStream(ViewEngine* ve, const QString& path, int flags, QObject *parent = nullptr);
    ~QmlDirectoryToStream();

    //QmlStreamProvider interface
    void wait() override;
    void resume() override;
    void next();

    lv::QmlStream* stream() const;

private:
    ViewEngine*    m_engine;
    QString        m_path;
    lv::QmlStream* m_stream;
    int            m_wait;
    int            m_flags;
    QDirIterator*  m_dit;
};

inline QmlStream *QmlDirectoryToStream::stream() const{
    return m_stream;
}

}// namespace

#endif // LVQMLDIRECTORYTOSTREAM_H
