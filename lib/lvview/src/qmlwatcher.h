#ifndef LVQMLWATCHER_H
#define LVQMLWATCHER_H

#include <QObject>
#include <QQmlParserStatus>

namespace lv{

class QmlWatcher : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit QmlWatcher(QObject *parent = nullptr);
    ~QmlWatcher() override;

protected:
    void classBegin() override{}
    void componentComplete() override;

signals:

public slots:
};

}// namespace

#endif // LVQMLWATCHER_H
