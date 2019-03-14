#ifndef LVGROUP_H
#define LVGROUP_H

#include "live/lvviewglobal.h"

#include <QObject>
#include <QQmlParserStatus>
#include <QQmlEngine>

namespace lv{

class LV_VIEW_EXPORT Group : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit Group(QObject *parent = nullptr);
    virtual ~Group();

    void classBegin(){}
    void componentComplete();

    static QByteArray typeDefaultValue(const QByteArray& typeString);
    static lv::Group* createWithProperties(QQmlEngine* engine, const std::map<QByteArray, QByteArray>& properties);

signals:
    void complete();

public slots:

private:
    bool m_isComponentComplete;

};

}// namespace

#endif // GROUP_H
