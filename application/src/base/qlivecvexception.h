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

#ifndef QLIVECVEXCEPTION_H
#define QLIVECVEXCEPTION_H

#include <exception>
#include <QString>

class QLiveCVException : public std::exception{

public:
    QLiveCVException(const QString& message, int code = 0);
    virtual ~QLiveCVException() throw();

    const QString& message() const throw();
    int code() const throw();

private:
    QString m_message;
    int     m_code;
};

inline const QString &QLiveCVException::message() const throw(){
    return m_message;
}

inline int QLiveCVException::code() const throw(){
    return m_code;
}

#endif // QLIVECVEXCEPTION_H
