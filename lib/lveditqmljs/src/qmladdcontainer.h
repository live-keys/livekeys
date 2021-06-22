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
    explicit QmlAddContainer(int addPosition, const QmlTypeReference& objectType, QObject *parent = nullptr);
    ~QmlAddContainer();

    lv::QmlSuggestionModel* model() const;

    QString objectType() const;

private:
    int                         m_objectBegin;
    QString                     m_objectIndentation;
    QmlTypeReference            m_objectType;
    lv::QmlSuggestionModel*     m_model;
};

inline QmlSuggestionModel* QmlAddContainer::model() const{
    return m_model;
}

inline QString QmlAddContainer::objectType() const{
    return m_objectType.join();
}

}// namespace

#endif // LVQMLADDCONTAINER_H
