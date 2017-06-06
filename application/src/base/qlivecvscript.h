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

#ifndef QLIVECVSCRIPT_H
#define QLIVECVSCRIPT_H

#include <QObject>
#include <QStringList>

namespace lcv{

class QProjectDocument;

class QLiveCVScript : public QObject{

    Q_OBJECT
    Q_PROPERTY(QStringList argv     READ argv     NOTIFY argvChanged)
    Q_PROPERTY(QStringList argvTail READ argvTail CONSTANT)

public:
    QLiveCVScript(const QStringList& argvTail, QObject* parent = 0);

    const QStringList& argv() const;
    const QStringList& argvTail() const;

public slots:
    void scriptChanged(QProjectDocument* active);
    const QString& name() const;

signals:
    void argvChanged();

private:
    QStringList m_argvTail;
    QStringList m_argv;

};

inline const QStringList &QLiveCVScript::argv() const{
    return m_argv;
}

inline const QStringList &QLiveCVScript::argvTail() const{
    return m_argvTail;
}

inline const QString &QLiveCVScript::name() const{
    return m_argv.first();
}

}// namespace

#endif // QLIVECVSCRIPT_H
