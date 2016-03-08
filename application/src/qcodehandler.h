/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/
#ifndef QCODEHANDLER_HPP
#define QCODEHANDLER_HPP

#include <QQuickItem>

class QQuickTextDocument;

class QCodeJSHighlighter;
class QCodeHandler : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QQuickTextDocument* target READ target WRITE setTarget NOTIFY targetChanged)

public:
    explicit QCodeHandler(QQuickItem *parent = 0);
    ~QCodeHandler();

    QQuickTextDocument *target();
    void setTarget(QQuickTextDocument *target);

signals:
    void targetChanged();

private:
    QQuickTextDocument*  m_target;
    QCodeJSHighlighter*  m_highlighter;

};

inline QQuickTextDocument *QCodeHandler::target(){
    return m_target;
}

#endif // QCODEHANDLER_HPP
