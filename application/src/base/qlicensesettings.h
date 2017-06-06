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

#ifndef QLICENSESETTINGS_H
#define QLICENSESETTINGS_H

#include <QObject>
#include <QHash>

namespace lcv{

class QLicenseContainer;
class QLicenseSettings{

public:
    explicit QLicenseSettings(const QString& licenseFile);
    ~QLicenseSettings();

public:
    void reparse();
    void save();
    QLicenseContainer* container();

private:
    QLicenseContainer* m_container;
    QString            m_licenseFile;
    bool               m_parseError;
    QString            m_errorText;
};

inline QLicenseContainer *QLicenseSettings::container(){
    return m_container;
}

}// namespace

#endif // QLICENSESETTINGS_H
