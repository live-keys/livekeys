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

#ifndef QMERGEROBERTSON_H
#define QMERGEROBERTSON_H

#include <QObject>
#include <QQmlParserStatus>
#include "qmatlist.h"
#include "opencv2/photo.hpp"

class QMergeRobertson : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QMatList* input    READ input    WRITE setInput    NOTIFY inputChanged)
    Q_PROPERTY(QList<qreal> times   READ times    WRITE setTimes    NOTIFY timesChanged)
    Q_PROPERTY(QMat* response     READ response WRITE setResponse NOTIFY responseChanged)
    Q_PROPERTY(QMat* output       READ output   NOTIFY outputChanged)

public:
    explicit QMergeRobertson(QObject *parent = nullptr);
    ~QMergeRobertson();

    QMatList* input() const;
    const QList<qreal>& times() const;
    QMat* output() const;
    QMat* response() const;

    void setInput(QMatList* input);
    void setTimes(QList<qreal> times);
    void setResponse(QMat* response);

    void classBegin() Q_DECL_OVERRIDE{}
    void componentComplete() Q_DECL_OVERRIDE;

signals:
    void inputChanged();
    void timesChanged();
    void outputChanged();
    void responseChanged();

private:
    void filter();
    bool isComponentComplete();

    QMatList*   m_input;
    QList<qreal>  m_times;
    QMat*       m_response;
    QMat*       m_output;

    cv::Ptr<cv::MergeRobertson> m_mergeRobertson;

    bool        m_componentComplete;
};

inline QMatList *QMergeRobertson::input() const{
    return m_input;
}

inline const QList<qreal> &QMergeRobertson::times() const{
    return m_times;
}

inline QMat *QMergeRobertson::output() const{
    return m_output;
}

inline void QMergeRobertson::setInput(QMatList *input){
    m_input = input;
    emit inputChanged();

    filter();
}

inline void QMergeRobertson::setTimes(QList<qreal> times){
    if (m_times == times)
        return;

    m_times = times;
    emit timesChanged();

    filter();
}

inline bool QMergeRobertson::isComponentComplete(){
    return m_componentComplete;
}

inline void QMergeRobertson::componentComplete(){
    m_componentComplete = true;
    filter();
}

inline void QMergeRobertson::setResponse(QMat *response){
    m_response = response;
    emit responseChanged();

    filter();
}

inline QMat *QMergeRobertson::response() const{
    return m_response;
}

#endif // QMERGEROBERTSON_H
