/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#ifndef QCALIBRATEROBERTSON_H
#define QCALIBRATEROBERTSON_H

#include <QObject>
#include <QQmlParserStatus>
#include "opencv2/photo.hpp"
#include "live/qmlobjectlist.h"
#include "qmat.h"

class QCalibrateRobertson : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(lv::QmlObjectList* input    READ input  WRITE setInput  NOTIFY inputChanged)
    Q_PROPERTY(QList<qreal> times   READ times  WRITE setTimes  NOTIFY timesChanged)
    Q_PROPERTY(QVariantMap params READ params WRITE setParams NOTIFY paramsChanged)
    Q_PROPERTY(QMat* output       READ output NOTIFY outputChanged)

public:
    explicit QCalibrateRobertson(QObject *parent = nullptr);
    ~QCalibrateRobertson();

    lv::QmlObjectList* input() const;
    const QList<qreal>& times() const;
    const QVariantMap& params() const;
    QMat* output() const;

    void setInput(lv::QmlObjectList* input);
    void setTimes(QList<qreal> times);

    void classBegin() Q_DECL_OVERRIDE{}
    void componentComplete() Q_DECL_OVERRIDE;

signals:
    void inputChanged();
    void timesChanged();
    void paramsChanged();
    void outputChanged();

public slots:
    void setParams(const QVariantMap& params);

private:
    void filter();
    bool isComponentComplete();

    lv::QmlObjectList*      m_input;
    QList<qreal>            m_times;
    QVariantMap             m_params;
    QMat*                   m_output;

    cv::Ptr<cv::CalibrateRobertson> m_calibrateRobertson;

    bool        m_componentComplete;
};

inline lv::QmlObjectList *QCalibrateRobertson::input() const{
    return m_input;
}

inline const QList<qreal> &QCalibrateRobertson::times() const{
    return m_times;
}

inline const QVariantMap &QCalibrateRobertson::params() const{
    return m_params;
}

inline QMat *QCalibrateRobertson::output() const{
    return m_output;
}

inline void QCalibrateRobertson::setInput(lv::QmlObjectList *input){
    m_input = input;
    emit inputChanged();

    filter();
}

inline void QCalibrateRobertson::setTimes(QList<qreal> times){
    if (m_times == times)
        return;

    m_times = times;
    emit timesChanged();

    filter();
}

inline bool QCalibrateRobertson::isComponentComplete(){
    return m_componentComplete;
}

inline void QCalibrateRobertson::componentComplete(){
    m_componentComplete = true;
    filter();
}
#endif // QCALIBRATEROBERTSON_H
