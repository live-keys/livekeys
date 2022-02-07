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

#ifndef LVQMLTOKENIZER_H
#define LVQMLTOKENIZER_H

#include <QObject>
#include <QJSValue>

namespace lv{

class ViewEngine;
class QmlTokenizer : public QObject{

    Q_OBJECT
    Q_PROPERTY(QJSValue tokenKind  READ tokenKind CONSTANT)
    Q_PROPERTY(QJSValue tokenState READ tokenState CONSTANT)

public:
    QmlTokenizer(QObject* parent = nullptr);
    ~QmlTokenizer();

    QJSValue tokenKind() const;
    QJSValue tokenState() const;

public slots:
    QJSValue scan(const QString& text);

private:
    QJSValue    m_tokenKind;
    QJSValue    m_tokenState;

    ViewEngine* m_engine;
};

inline QJSValue QmlTokenizer::tokenKind() const{
    return m_tokenKind;
}

inline QJSValue QmlTokenizer::tokenState() const{
    return m_tokenState;
}

}// namespace

#endif // LVQMLTOKENIZER_H
