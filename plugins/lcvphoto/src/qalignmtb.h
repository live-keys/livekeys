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
#include "qmatlist.h"
#include "opencv2/photo.hpp"

class QAlignMTB : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QMatList*    input  READ input  WRITE  setInput  NOTIFY inputChanged)
    Q_PROPERTY(QVariantMap  params READ params WRITE  setParams NOTIFY paramsChanged)
    Q_PROPERTY(QMatList*    output READ output NOTIFY outputChanged)

public:
    explicit QAlignMTB(QObject *parent = nullptr);
    ~QAlignMTB();

    QMatList* input() const;
    QMatList* output() const;

    void setInput(QMatList* input);

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

    QMatList*             m_input;
    QMatList*             m_output;
    cv::Ptr<cv::AlignMTB> m_alignMTB;
    QVariantMap           m_params;
    bool                  m_componentComplete;
};

inline QMatList *QAlignMTB::input() const{
    return m_input;
}

inline QMatList *QAlignMTB::output() const{
    return m_output;
}

inline void QAlignMTB::setInput(QMatList *input){
    m_input = input;
    emit inputChanged();

    filter();
}

inline const QVariantMap& QAlignMTB::params() const{
    return m_params;
}

#endif // QALIGNMTB_H
