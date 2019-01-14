#ifndef LVQMLADDCONTAINER_H
#define LVQMLADDCONTAINER_H

#include "live/lveditqmljsglobal.h"
#include "qmlpropertymodel.h"
#include "qmlitemmodel.h"

#include <QObject>

namespace lv{


/// \private
class QmlAddContainer : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::QmlItemModel*     itemModel     READ itemModel     CONSTANT)
    Q_PROPERTY(lv::QmlPropertyModel* propertyModel READ propertyModel CONSTANT)
    Q_PROPERTY(QString               objectType    READ objectType    CONSTANT)

public:
    explicit QmlAddContainer(int addPosition, const QStringList& objectType, QObject *parent = 0);
    ~QmlAddContainer();

    lv::QmlItemModel* itemModel() const;
    lv::QmlPropertyModel* propertyModel() const;
    QString objectType() const;

private:
    QStringList           m_objectTypePath;
    lv::QmlItemModel*     m_itemModel;
    lv::QmlPropertyModel* m_propertyModel;
};

inline QmlItemModel* QmlAddContainer::itemModel() const{
    return m_itemModel;
}

inline QmlPropertyModel* QmlAddContainer::propertyModel() const{
    return m_propertyModel;
}

inline QString QmlAddContainer::objectType() const{
    return m_objectTypePath.join(".");
}

}// namespace

#endif // LVQMLADDCONTAINER_H
