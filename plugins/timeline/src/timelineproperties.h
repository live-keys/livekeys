#ifndef TIMELINEPROPERTIES_H
#define TIMELINEPROPERTIES_H

#include <QObject>
#include <QJSValue>
#include <QQmlPropertyMap>

namespace lv{

class TimelineProperties : public QQmlPropertyMap{

    Q_OBJECT

public:
    explicit TimelineProperties(QObject *parent = nullptr);
    ~TimelineProperties();

    void insertAndCheckValue(const QString& key, const QVariant& input);

protected:
    virtual QVariant updateValue(const QString &key, const QVariant &input) override;

};

}// namespace

#endif // TIMELINEPROPERTIES_H
