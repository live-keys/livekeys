#ifndef LVQMLSTREAMVALUEAWAIT_H
#define LVQMLSTREAMVALUEAWAIT_H

#include <QObject>
#include "live/qmlstream.h"
#include "live/qmlact.h"

namespace lv{

class QmlStreamValueAwait : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(lv::QmlStream* stream READ stream    WRITE setStream NOTIFY streamChanged)
    Q_PROPERTY(QString valueType     READ valueType WRITE setValueType NOTIFY valueTypeChanged)
    Q_PROPERTY(lv::QmlAct* act       READ act       WRITE setAct NOTIFY actChanged)
    Q_PROPERTY(QJSValue value        READ value     NOTIFY valueChanged)

public:
    explicit QmlStreamValueAwait(QObject *parent = nullptr);
    ~QmlStreamValueAwait();

    lv::QmlStream* stream() const;
    void setStream(lv::QmlStream* stream);

    QJSValue value() const;

    static void streamHandler(QObject* that, const QJSValue& val);

    void classBegin();
    void componentComplete();

    const QString& valueType() const;
    void setValueType(const QString& valueType);

    QmlAct* act() const;
    void setAct(lv::QmlAct* act);

public slots:
    void __actResultReady();
    void __updateFollowsObject();

signals:
    void streamChanged();
    void valueChanged();

    void valueTypeChanged();

    void actChanged();

private:
    void onStreamValue(const QJSValue& val);
    void removeFollowsObject();

    bool           m_isComponentComplete;
    bool           m_isRunning;
    bool           m_inStreamValue;
    QObject*       m_follow;
    lv::QmlStream* m_stream;
    lv::QmlAct*    m_act;
    QJSValue       m_current;
    QJSValue*      m_next;
    QString        m_valueType;
};

inline QmlStream *QmlStreamValueAwait::stream() const{
    return m_stream;
}

inline QJSValue QmlStreamValueAwait::value() const{
    return m_current;
}

inline const QString &QmlStreamValueAwait::valueType() const{
    return m_valueType;
}

inline QmlAct *QmlStreamValueAwait::act() const{
    return m_act;
}

}// namespace

#endif // LVQMLSTREAMVALUEAWAIT_H
