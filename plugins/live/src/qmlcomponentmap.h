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

#ifndef LVQMLCOMPONENTMAP_H
#define LVQMLCOMPONENTMAP_H

#include <QObject>
#include <QQmlComponent>

namespace lv{

class QmlComponentMapData;

/// \private
class QmlComponentMap : public QObject{

    Q_OBJECT
    Q_PROPERTY(QVariantList input  READ input  WRITE setInput  NOTIFY inputChanged)
    Q_PROPERTY(QQmlComponent* f    READ f      WRITE setF      NOTIFY fChanged)
    Q_PROPERTY(QVariantList output READ output NOTIFY outputChanged)

public:
    explicit QmlComponentMap(QObject *parent = nullptr);
    ~QmlComponentMap();

    void setInput(const QVariantList& input);
    void setF(QQmlComponent* f);

    const QVariantList& input() const;
    const QVariantList& output() const;
    QQmlComponent* f() const;

    void process();

    void assignResult(QmlComponentMapData* mad, const QVariant& v);

    void clearCurrent();

signals:
    void inputChanged();
    void fChanged();
    void outputChanged();

private:
    QVariantList   m_input;
    QVariantList   m_output;

    QList<QObject*>      m_fObjects;
    QList<QQmlContext*>  m_fContexts;
    QList<QmlComponentMapData*> m_fData;

    bool                 m_isProcessing;

    QQmlComponent* m_f;
};

inline const QVariantList &QmlComponentMap::input() const{
    return m_input;
}

inline QQmlComponent *QmlComponentMap::f() const{
    return m_f;
}

inline void QmlComponentMap::setInput(const QVariantList &input){
    if (m_input == input)
        return;

    process();
    m_input = input;
    emit inputChanged();
}

inline void QmlComponentMap::setF(QQmlComponent *f){
    if (m_f == f)
        return;

    process();
    m_f = f;
    emit fChanged();
}

inline const QVariantList &QmlComponentMap::output() const{
    return m_output;
}

}// namespace

#endif // LVQMLCOMPONENTMAP_H
