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

#ifndef LVQMLEDITFRAGMENTCONTAINER_H
#define LVQMLEDITFRAGMENTCONTAINER_H

#include <QObject>
#include <QLinkedList>
#include <QJSValue>

#include "lveditqmljsglobal.h"

namespace lv{

class QmlEditFragment;
class LanguageQmlHandler;

class LV_EDITQMLJS_EXPORT QmlEditFragmentContainer : public QObject{

    Q_OBJECT
    Q_PROPERTY(int editCount READ editCount NOTIFY editCountChanged)

public:
    friend class LanguageQmlHandler;

public:
    explicit QmlEditFragmentContainer(LanguageQmlHandler *parent = nullptr);
    ~QmlEditFragmentContainer() override;

    int editCount() const;

public slots:
    bool addEdit(lv::QmlEditFragment* edit);
    void derefEdit(lv::QmlEditFragment* edit);
    void removeEdit(lv::QmlEditFragment* edit);
    QJSValue allEdits();
    void clearAllFragments();

    lv::QmlEditFragment* topEditAtPosition(int position);

    //TOMOVE
    lv::QmlEditFragment* findObjectFragmentByPosition(int position);
    lv::QmlEditFragment* findFragmentByPosition(int position);

signals:
    void editCountChanged();

private:
    QLinkedList<QmlEditFragment*> m_edits; // opened fragments
    LanguageQmlHandler*               m_codeHandler;
};

inline int QmlEditFragmentContainer::editCount() const{
    return m_edits.size();
}

}// namespace

#endif // LVQMLEDITFRAGMENTCONTAINER_H
