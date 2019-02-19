#ifndef LVGROUP_H
#define LVGROUP_H

#include <QObject>
#include <QQmlParserStatus>

namespace lv{

/// \private
class Group : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit Group(QObject *parent = nullptr);
    virtual ~Group();

    void classBegin(){}
    void componentComplete();

signals:
    void complete();

public slots:

private:
    bool m_isComponentComplete;

};

}// namespace

#endif // GROUP_H
