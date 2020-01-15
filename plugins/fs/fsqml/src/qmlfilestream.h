#ifndef LVQMLFILESTREAM_H
#define LVQMLFILESTREAM_H

#include <QObject>
#include <QTextStream>
#include <QFile>

#include "live/qmlstream.h"

namespace lv{

class QmlFileStream : public QObject{

    Q_OBJECT

public:
    explicit QmlFileStream(QObject *parent = nullptr);
    ~QmlFileStream();

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
    bool        m_synced;
};

inline QmlFileStream *QmlFileStream::synced(){
    m_synced = true;
    return this;
}

}// namespace

#endif // LVQMLFILESTREAM_H
