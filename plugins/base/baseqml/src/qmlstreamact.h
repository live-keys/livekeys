#ifndef LVQMLSTREAMACT_H
#define LVQMLSTREAMACT_H

#include <QObject>
#include "live/qmlact.h"
#include "live/qmlstream.h"
#include "live/qmlstreamprovider.h"

namespace lv{

class QmlStreamAct : public QObject, public QmlStreamProvider{

    Q_OBJECT
    Q_PROPERTY(lv::QmlStream* input  READ input        WRITE setInput       NOTIFY inputChanged)
    Q_PROPERTY(lv::QmlAct* act       READ act          WRITE setAct         NOTIFY actChanged)
    Q_PROPERTY(QString actProperty   READ actProperty  WRITE setActProperty NOTIFY actPropertyChanged)
    Q_PROPERTY(lv::QmlStream* output READ output       NOTIFY outChanged)

public:
    explicit QmlStreamAct(QObject *parent = nullptr);
    virtual ~QmlStreamAct();

    lv::QmlStream* input() const;
    void setInput(lv::QmlStream* stream);

    lv::QmlStream* output() const;

    // QmlStreamProvider interface
    void wait() override;
    void resume() override;

    lv::QmlAct* act() const;
    void setAct(lv::QmlAct* act);

    const QString &actProperty() const;
    void setActProperty(const QString& argument);

    static void streamHandler(QObject* that, const QJSValue& val);

public slots:
    void __actResultReady();

signals:
    void inputChanged();
    void outChanged();
    void actChanged();
    void actPropertyChanged();

private:
    void initMetaProperty();
    void onStreamValue(const QJSValue& val);

    lv::QmlStream* m_input;
    lv::QmlStream* m_output;
    lv::QmlAct*    m_act;
    QString        m_actProperty;
    QMetaProperty  m_propertyMeta;
    bool           m_isRunning;
};

inline QmlStream *QmlStreamAct::input() const{
    return m_input;
}

inline QmlStream *QmlStreamAct::output() const{
    return m_output;
}

inline QmlAct *QmlStreamAct::act() const{
    return m_act;
}

inline const QString& QmlStreamAct::actProperty() const{
    return m_actProperty;
}

}// namespace

#endif // LVQMLSTREAMACT_H
