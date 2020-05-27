#ifndef LVQMLADDCONTAINER_H
#define LVQMLADDCONTAINER_H

#include "live/lveditqmljsglobal.h"
#include "qmlsuggestionmodel.h"
#include <QObject>

namespace lv{


/// \private
class QmlAddContainer : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::QmlSuggestionModel*      itemModel       READ itemModel      CONSTANT)
    Q_PROPERTY(lv::QmlSuggestionModel*      propertyModel   READ propertyModel  CONSTANT)
    Q_PROPERTY(lv::QmlSuggestionModel*      eventModel      READ eventModel     CONSTANT)
    Q_PROPERTY(lv::QmlSuggestionModel*      functionModel   READ functionModel  CONSTANT)
    Q_PROPERTY(QString                      objectType      READ objectType     CONSTANT)

public:
    explicit QmlAddContainer(int addPosition, const QStringList& objectType, QObject *parent = 0);
    ~QmlAddContainer();

    lv::QmlSuggestionModel* itemModel() const;
    lv::QmlSuggestionModel* propertyModel() const;
    lv::QmlSuggestionModel* eventModel() const;
    lv::QmlSuggestionModel* functionModel() const;

    QString objectType() const;

private:
    int                         m_objectBegin;
    QString                     m_objectIndentation;
    QStringList                 m_objectTypePath;
    lv::QmlSuggestionModel*     m_itemModel;
    lv::QmlSuggestionModel*     m_propertyModel;
    lv::QmlSuggestionModel*     m_eventModel;
    lv::QmlSuggestionModel*     m_functionModel;
};

inline QmlSuggestionModel* QmlAddContainer::itemModel() const{
    return m_itemModel;
}

inline QmlSuggestionModel* QmlAddContainer::propertyModel() const{
    return m_propertyModel;
}

inline QmlSuggestionModel* QmlAddContainer::eventModel() const{
    return m_eventModel;
}

inline QmlSuggestionModel* QmlAddContainer::functionModel() const{
    return m_functionModel;
}

inline QString QmlAddContainer::objectType() const{
    return m_objectTypePath.join(".");
}

}// namespace

#endif // LVQMLADDCONTAINER_H
