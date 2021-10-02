#ifndef LVQMLSTREAMSINK_H
#define LVQMLSTREAMSINK_H

#include <QObject>
#include <QJSValue>
#include <QQmlParserStatus>

namespace lv{

class QmlStream;
class QmlStreamSink : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QJSValue input READ input WRITE setInput NOTIFY inputChanged)

public:
    explicit QmlStreamSink(QObject *parent = nullptr);
    ~QmlStreamSink();

    const QJSValue& input() const;

    void classBegin(){}
    void componentComplete();

public slots:
    void setInput(const QJSValue& input);

signals:
    void inputChanged();

private:
    QJSValue   m_input;
    QmlStream* m_target;
};

inline const QJSValue &QmlStreamSink::input() const{
    return m_input;
}

}// namespace

#endif // LVQMLSTREAMSINK_H
