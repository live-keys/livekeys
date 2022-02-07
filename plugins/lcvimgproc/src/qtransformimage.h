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

#ifndef QTRANSFORMIMAGE_H
#define QTRANSFORMIMAGE_H

#include <QObject>
#include <QJSValue>
#include <QQmlListProperty>

class QTransformImage : public QObject{

    Q_OBJECT
    Q_PROPERTY(QObject* input                  READ input WRITE setInput NOTIFY inputChanged)
    Q_PROPERTY(QJSValue result                 READ result NOTIFY resultChanged)
    Q_PROPERTY(QQmlListProperty<QObject> items READ items CONSTANT)
    Q_CLASSINFO("DefaultProperty", "items")

public:
    explicit QTransformImage(QObject *parent = nullptr);

    QObject* input() const;
    void setInput(QObject* input);
    QQmlListProperty<QObject> items();

    static void appendObjectToList(QQmlListProperty<QObject>*, QObject*);
    static int objectCount(QQmlListProperty<QObject>*);
    static QObject* objectAt(QQmlListProperty<QObject>*, int);
    static void clearObjects(QQmlListProperty<QObject>*);

    QJSValue result() const;

    void run();

signals:
    void inputChanged();
    void resultChanged();

public slots:
    void exec();
    void __childDestroyed();
    void __childResultChanged();

private:
    QObject*        m_input;
    QList<QObject*> m_children;
    QJSValue        m_result;
};

inline QObject *QTransformImage::input() const{
    return m_input;
}

inline void QTransformImage::exec(){
    run();
}

#endif // QTRANSFORMIMAGE_H
