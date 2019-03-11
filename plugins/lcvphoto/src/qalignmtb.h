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

#ifndef QALIGNMTB_H
#define QALIGNMTB_H

#include <QObject>
#include <QQmlParserStatus>
#include "opencv2/photo.hpp"
#include "live/qmlobjectlist.h"

class QAlignMTB : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(lv::QmlObjectList*    input  READ input  WRITE  setInput    NOTIFY inputChanged)
    Q_PROPERTY(QVariantMap           params READ params WRITE  setParams   NOTIFY paramsChanged)
    Q_PROPERTY(lv::QmlObjectList*    output READ output WRITE  setupOutput NOTIFY outputChanged)

public:
    explicit QAlignMTB(QObject *parent = nullptr);
    ~QAlignMTB();

    lv::QmlObjectList* input() const;
    lv::QmlObjectList* output() const;

    void setInput(lv::QmlObjectList* input);
    void setupOutput(lv::QmlObjectList* output);
    void classBegin() Q_DECL_OVERRIDE{}
    void componentComplete() Q_DECL_OVERRIDE;

    const QVariantMap& params() const;

public slots:
    void setParams(const QVariantMap& params);

signals:
    void inputChanged();
    void outputChanged();
    void paramsChanged();

private:
    void filter();
    bool isComponentComplete();

    lv::QmlObjectList*    m_input;
    lv::QmlObjectList*    m_output;
    cv::Ptr<cv::AlignMTB> m_alignMTB;
    QVariantMap           m_params;
    bool                  m_componentComplete;
};

inline lv::QmlObjectList *QAlignMTB::input() const{
    return m_input;
}

inline lv::QmlObjectList *QAlignMTB::output() const{
    return m_output;
}

inline void QAlignMTB::setInput(lv::QmlObjectList *input){
    m_input = input;
    emit inputChanged();

    filter();
}

inline void QAlignMTB::setupOutput(lv::QmlObjectList *output){
    m_output = output;
    filter();
}

inline const QVariantMap& QAlignMTB::params() const{
    return m_params;
}

#endif // QALIGNMTB_H
