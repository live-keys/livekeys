/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#ifndef LVVISUALLOGQT_H
#define LVVISUALLOGQT_H

#include "live/visuallog.h"

#include <QString>
#include <QTextStream>

#include <ostream>


inline std::ostream& operator << (std::ostream &stream, const QChar& val ){
    stream << val.toLatin1();
    return stream;
}

inline std::ostream& operator << (std::ostream &stream, const QString& val ){
    stream << val.toStdString();
    return stream;
}

inline std::ostream& operator << (std::ostream &stream, const QByteArray& val ){
    stream << val.data();
    return stream;
}

inline std::ostream& operator << (std::ostream &stream, const QStringRef& val ){
    QByteArray utf8 = val.toUtf8();
    stream << utf8.data();
    return stream;
}

namespace lv{

    inline void visualLogMessageHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& msg){
        const char* file = ctx.file     ? ctx.file : "";
        const char* func = ctx.function ? ctx.function : "";
        switch (type){
        case QtInfoMsg:     VisualLog().at(file, ctx.line, func).i() << msg; break;
        case QtDebugMsg:    VisualLog().at(file, ctx.line, func).d() << msg; break;
        case QtWarningMsg:  VisualLog().at(file, ctx.line, func).w() << msg; break;
        case QtCriticalMsg: VisualLog().at(file, ctx.line, func).e() << msg; break;
        case QtFatalMsg:    VisualLog().at(file, ctx.line, func).f() << msg; break;
        }
    }

}

#endif // LVVISUALLOGQT_H
