#ifndef LVQMLSTREAMVALUE_H
#define LVQMLSTREAMVALUE_H

#include <QObject>
#include "live/qmlstream.h"

namespace lv{

class QmlStreamValue : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(lv::QmlStream* stream READ stream    WRITE setStream NOTIFY streamChanged)
    Q_PROPERTY(QString valueType     READ valueType WRITE setValueType NOTIFY valueTypeChanged)
    Q_PROPERTY(QJSValue value        READ value     NOTIFY valueChanged)

public:
    explicit QmlStreamValue(QObject *parent = nullptr);
    ~QmlStreamValue();

    lv::QmlStream* stream() const;
    void setStream(lv::QmlStream* stream);

    QJSValue value() const;

    static void streamHandler(QObject* that, const QJSValue& val);

    void classBegin();
    void componentComplete();

    const QString& valueType() const;
    void setValueType(const QString& valueType);

public slots:
    void __streamRemoved();

signals:
    void streamChanged();
    void valueChanged();

    void valueTypeChanged(QString valueType);

private:
    void updateFollowsObject();
    void removeFollowsObject();

    bool           m_isComponentComplete;
    QObject*       m_follow;
    lv::QmlStream* m_stream;
    QJSValue       m_current;
    QString        m_valueType;
};

inline QmlStream *QmlStreamValue::stream() const{
    return m_stream;
}

inline QJSValue QmlStreamValue::value() const{
    return m_current;
}

inline const QString &QmlStreamValue::valueType() const{
    return m_valueType;
}

}// namespace

#endif // LVQMLSTREAMVALUE_H
