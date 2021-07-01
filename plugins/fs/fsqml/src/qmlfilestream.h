#ifndef LVQMLFILESTREAM_H
#define LVQMLFILESTREAM_H

#include <QObject>
#include <QTextStream>
#include <QFile>

#include "live/qmlstream.h"
#include "live/qmlstreamprovider.h"

namespace lv{

class QmlFileStream : public QObject, public QmlStreamProvider{

    Q_OBJECT

public:
    explicit QmlFileStream(QObject *parent = nullptr);
    ~QmlFileStream();

    // QmlStreamProvider interface
    void wait() override;
    void resume() override;

signals:

public slots:
    void next();
    lv::QmlFileStream* synced();
    lv::QmlStream* lines(const QString& file);

private:
    QmlStream*  m_stream;
    QFile       m_file;
    QTextStream m_text;
    QString     m_filePath;
    int         m_wait;

};

inline QmlFileStream *QmlFileStream::synced(){
    m_wait = true;
    return this;
}

}// namespace

#endif // LVQMLFILESTREAM_H
