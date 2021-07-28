#ifndef LVQMLDIRECTORYREAD_H
#define LVQMLDIRECTORYREAD_H

#include <QObject>
#include <QDir>
#include <QDirIterator>

#include "live/qmlstream.h"
#include "live/qmlstreamprovider.h"

namespace lv{

class QmlDirectoryRead : public QObject, public QmlStreamProvider{

    Q_OBJECT
    Q_PROPERTY(QString path          READ path   WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(int flags             READ flags  WRITE setFlags NOTIFY flagsChanged)
    Q_PROPERTY(lv::QmlStream* stream READ stream NOTIFY streamChanged)

    Q_ENUMS(Flags)

public:
    enum Flags{
        NoFlags = 0,
        Subdirectories = 0x2,
        FollowSymlinks = 0x1
    };

public:
    explicit QmlDirectoryRead(QObject *parent = nullptr);
    ~QmlDirectoryRead();

    //QmlStreamProvider interface
    void wait() override;
    void resume() override;
    void next();

    const QString &path() const;
    void setPath(const QString& path);

    lv::QmlStream* stream() const;

    int flags() const;
    void setFlags(int flags);

public slots:
    void start();
    void stop();

signals:
    void pathChanged();
    void streamChanged();
    void flagsChanged();

private:
    QString        m_path;
    lv::QmlStream* m_stream;
    int            m_wait;
    int            m_flags;
    QDirIterator*  m_dit;
};

inline const QString& QmlDirectoryRead::path() const{
    return m_path;
}

inline QmlStream *QmlDirectoryRead::stream() const{
    return m_stream;
}

inline int QmlDirectoryRead::flags() const{
    return m_flags;
}

inline void QmlDirectoryRead::setFlags(int flags){
    if (m_flags == flags)
        return;
    m_flags = flags;
    emit flagsChanged();
}

}// namespace

#endif // LVQMLDIRECTORYREAD_H
