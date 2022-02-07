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

#ifndef LVDOCUMENTATIONLOADER_H
#define LVDOCUMENTATIONLOADER_H

#include <QObject>
#include <QJSValue>
#include "live/lveditorglobal.h"

namespace lv{

class LV_EDITOR_EXPORT DocumentationLoader : public QObject{

    Q_OBJECT
    Q_PROPERTY(QJSValue load READ load WRITE setLoad NOTIFY loadChanged)

public:
    explicit DocumentationLoader(QObject *parent = nullptr);
    ~DocumentationLoader(){}

    const QJSValue &load() const;
    void setLoad(const QJSValue &load);

signals:
    void loadChanged();

private:
    QJSValue m_load;
};

inline const QJSValue& DocumentationLoader::load() const{
    return m_load;
}

inline void DocumentationLoader::setLoad(const QJSValue& load){
    m_load = load;
    emit loadChanged();
}

}// namespace

#endif // LVDOCUMENTATIONLOADER_H
