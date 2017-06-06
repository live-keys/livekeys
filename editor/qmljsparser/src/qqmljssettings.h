/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#ifndef QQMLJSSETTINGS_H
#define QQMLJSSETTINGS_H

#include "qeditorsettingscategory.h"

namespace lcv{

class QQmlJsHighlighter;
class QQmlJsSettings : public QEditorSettingsCategory{

public:
    QQmlJsSettings(QQmlJsHighlighter* highlighter);
    ~QQmlJsSettings();

    void fromJson(const QJsonValue &json) Q_DECL_OVERRIDE;
    QJsonValue toJson() const Q_DECL_OVERRIDE;

private:
    QQmlJsHighlighter* m_highlighter;

};

}// namespace

#endif // QQMLJSSETTINGS_H
