#ifndef QSTATICFILEREADER_H
#define QSTATICFILEREADER_H

#include <QQuickItem>
#include "qliveglobal.h"

class QJSValue;
class QFileReader;
class Q_LIVE_EXPORT QStaticFileReader : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QByteArray data READ data NOTIFY dataChanged)

public:
    explicit QStaticFileReader(QQuickItem *parent = 0);
    virtual ~QStaticFileReader();

    QByteArray data() const;

signals:
    void dataChanged(const QByteArray& data);
    void init();

protected:
    void componentComplete();

public slots:
    void staticLoad(const QString& file, const QJSValue& params = QJSValue());
    QString asString(const QByteArray& data);
    void readerDataChanged(const QByteArray& data);

private:
    QFileReader* m_reader;
    QByteArray   m_data;
};

inline QByteArray QStaticFileReader::data() const{
    return m_data;
}

inline QString QStaticFileReader::asString(const QByteArray &data){
    return QString::fromUtf8(data);
}

#endif // QSTATICFILEREADER_H
