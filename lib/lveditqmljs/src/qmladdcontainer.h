#ifndef LVQMLADDCONTAINER_H
#define LVQMLADDCONTAINER_H

#include "live/lveditqmljsglobal.h"
#include "qmlsuggestionmodel.h"
#include <QObject>

namespace lv{


/// \private
class QmlAddContainer : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::QmlSuggestionModel*      model           READ model          CONSTANT)
    Q_PROPERTY(QString                      objectType      READ objectType     CONSTANT)

public:
    explicit QmlAddContainer(int addPosition, const QStringList& objectType, QObject *parent = 0);
    ~QmlAddContainer();

    lv::QmlSuggestionModel* model() const;

    QString objectType() const;

private:
    int                         m_objectBegin;
    QString                     m_objectIndentation;
    QStringList                 m_objectTypePath;
    lv::QmlSuggestionModel*     m_model;
};

inline QmlSuggestionModel* QmlAddContainer::model() const{
    return m_model;
}

inline QString QmlAddContainer::objectType() const{
    return m_objectTypePath.join(".");
}

}// namespace

#endif // LVQMLADDCONTAINER_H
