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

#ifndef QPROJECTIONMAPPER_H
#define QPROJECTIONMAPPER_H

#include <QObject>
#include <QJSValue>
#include "qprojectionsurface.h"
#include <QQmlListProperty>

class QMat;

class QProjectionMapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QMat*                    backgroundImage     READ backgroundImage    WRITE   setBackgroundImage      NOTIFY backgroundImageChanged)
    Q_PROPERTY(QMat*                    result              READ result             NOTIFY  resultChanged)
    Q_PROPERTY(QMat*                    liveResult          READ liveResult         NOTIFY  liveResultChanged)
    Q_PROPERTY(QJSValue                 model               READ model              NOTIFY  modelChanged)
    Q_PROPERTY(QProjectionSurface*      focusSurface        READ focusSurface       NOTIFY focusSurfaceChanged)
    
    Q_PROPERTY(QQmlListProperty<QObject> surfaces READ surfaces CONSTANT)
    Q_CLASSINFO("DefaultProperty", "surfaces")
public:
    explicit QProjectionMapper(QObject *parent = nullptr);
    ~QProjectionMapper();

    QMat *backgroundImage() const;
    void setBackgroundImage(QMat *backgroundImage);

    void removeSurface(QProjectionSurface* surface);
    void addSurface(QProjectionSurface* surface);
    int numOfSurfaces();
    QProjectionSurface* surfaceAt(int idx);

    QMat *result() const;
    QMat *liveResult() const;

    QJSValue model() const;
    QQmlListProperty<QObject> surfaces();

    QProjectionSurface *focusSurface() const;

public slots:
    void modifyLiveResult(QJSValue points);
    void createResult();
    QProjectionSurface* selectFocusSurface(int idx);

signals:
    void backgroundImageChanged();
    void resultChanged();
    void liveResultChanged();
    void modelChanged();
    void focusSurfaceChanged();
private:
    void updateModel();
    static void surfacesAppend(QQmlListProperty<QObject> *p, QObject *v);
    static int surfacesCount(QQmlListProperty<QObject> *p);
    static QObject* surfacesAt(QQmlListProperty<QObject> *p, int idx);
    static void surfacesClear(QQmlListProperty<QObject> *p);
    QMat* applySurfaces(bool skipFocused = false, int idx = -1);

    QList<QProjectionSurface*> m_surfaces;
    QProjectionSurface* m_focusSurface;
    QMat* m_backgroundImage;
    QMat* m_result;
    QMat* m_liveResult;
    QMat* m_basis;
    QJSValue m_model;

};

#endif // QPROJECTIONMAPPER_H
