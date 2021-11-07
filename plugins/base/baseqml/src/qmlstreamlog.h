#ifndef LVQMLSTREAMLOG_H
#define LVQMLSTREAMLOG_H

#include <QObject>
#include "live/qmlstream.h"
#include "live/qmlvisuallog.h"

namespace lv{

class QmlStreamLog : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString prefix        READ prefix WRITE setPrefix NOTIFY prefixChanged)
    Q_PROPERTY(lv::QmlStream* stream READ stream WRITE setStream NOTIFY streamChanged)

public:
    explicit QmlStreamLog(QObject *parent = nullptr);
    ~QmlStreamLog();

    QmlStream* stream() const;
    void setStream(QmlStream* stream);

    const QString& prefix() const;
    void setPrefix(const QString& prefix);

    static void onStreamData(QObject* that, const QJSValue& val);

signals:
    void streamChanged();
    void prefixChanged();

private:
    QmlStream* m_stream;
    QString    m_prefix;
};

inline QmlStream *QmlStreamLog::stream() const{
    return m_stream;
}

inline const QString &QmlStreamLog::prefix() const{
    return m_prefix;
}

inline void QmlStreamLog::setPrefix(const QString &prefix){
    if (m_prefix == prefix)
        return;

    m_prefix = prefix;
    emit prefixChanged();
}

}// namespace

#endif // LVQMLSTREAMLOG_H
