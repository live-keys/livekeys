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

namespace lv{

class Environment;
class ProjectDocument;

class LiveCVScript : public QObject{

    Q_OBJECT
    Q_PROPERTY(QStringList  argv             READ argv        NOTIFY argvChanged)
    Q_PROPERTY(QStringList  argvTail         READ argvTail    CONSTANT)
    Q_PROPERTY(lv::Environment* environment READ environment CONSTANT)

public:
    LiveCVScript(const QStringList& argvTail, QObject* parent = 0);
    ~LiveCVScript();

    const QStringList& argv() const;
    const QStringList& argvTail() const;

    lv::Environment* environment();

public slots:
    void scriptChanged(lv::ProjectDocument* active);
    const QString& name() const;

signals:
    void argvChanged();

private:
    QStringList       m_argvTail;
    QStringList       m_argv;
    lv::Environment* m_environment;

};

inline const QStringList &LiveCVScript::argv() const{
    return m_argv;
}

inline const QStringList &LiveCVScript::argvTail() const{
    return m_argvTail;
}

inline Environment *LiveCVScript::environment(){
    return m_environment;
}

inline const QString &LiveCVScript::name() const{
    return m_argv.first();
}

}// namespace

#endif // QLIVECVSCRIPT_H
