#ifndef LVADDCONTAINER_H
#define LVADDCONTAINER_H

#include "lvpropertymodel.h"
#include "lvcomponentmodel.h"

#include <QObject>

namespace lv{


/// \private
class LvAddContainer : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::LvComponentModel*     componentModel     READ componentModel     CONSTANT)
    Q_PROPERTY(lv::LvPropertyModel*      propertyModel      READ propertyModel      CONSTANT)
    Q_PROPERTY(QString                   objectType         READ objectType         CONSTANT)

public:
    explicit LvAddContainer(int addPosition, const QStringList& objectType, QObject *parent = 0);
    ~LvAddContainer();

    lv::LvComponentModel* componentModel() const;
    lv::LvPropertyModel*  propertyModel() const;
    QString objectType() const;

private:
    int                       m_objectBegin;
    QString                   m_objectIndentation;
    QStringList               m_objectTypePath;
    lv::LvComponentModel*     m_componentModel;
    lv::LvPropertyModel*      m_propertyModel;
};

inline LvComponentModel* LvAddContainer::componentModel() const{
    return m_componentModel;
}

inline LvPropertyModel* LvAddContainer::propertyModel() const{
    return m_propertyModel;
}

inline QString LvAddContainer::objectType() const{
    return m_objectTypePath.join(".");
}

}// namespace

#endif // LVQMLADDCONTAINER_H
