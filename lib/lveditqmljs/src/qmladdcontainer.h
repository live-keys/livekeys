/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

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
