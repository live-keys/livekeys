#ifndef QMLTHREADINFO_H
#define QMLTHREADINFO_H

#include <QObject>

namespace lv{

class QmlThreadInfo : public QObject{

    Q_OBJECT

public:
    explicit QmlThreadInfo(QObject *parent = nullptr);
    ~QmlThreadInfo();

public slots:
    QString currentThreadIdString() const;
    QString objectThreadIdString(QObject* object) const;
};

}// namespace

#endif // QMLTHREADINFO_H
