#ifndef LVTUPLE_H
#define LVTUPLE_H

#include <QObject>
#include <QQmlParserStatus>
#include <QQmlProperty>
#include "live/shared.h"
#include "live/qmlact.h"
#include "live/mlnode.h"
#include "live/viewengine.h"

namespace lv{

/// \private
class Tuple : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

public:
    /// \private
    class PropertyDescription{
    public:
        QQmlProperty::PropertyTypeCategory typeCategory;
        QByteArray typeClass;
    };

public:
    explicit Tuple(QObject *parent = 0);
    ~Tuple();

    void classBegin(){}
    void componentComplete();

    bool reserveForRead(Shared::RefScope* locker, QmlAct* filter);

    int totalProperties();
    QHash<QByteArray, int>::ConstIterator propertiesBegin();
    QHash<QByteArray, int>::ConstIterator propertiesEnd();

    static void serialize(lv::ViewEngine* engine, const Tuple& t, MLNode& node);
    static void deserialize(lv::ViewEngine* engine, const MLNode& node, Tuple& t);
    static void deserialize(lv::ViewEngine* engine, const MLNode& node, QVariant& v);

private:
    bool                            m_componentComplete;
    QHash<QByteArray, int>          m_properties;
    QHash<int, PropertyDescription> m_propertyTypes;
};

}// namespace

#endif // LVTUPLE_H
