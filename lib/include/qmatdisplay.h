/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#ifndef QMATDISPLAY_HPP
#define QMATDISPLAY_HPP

#include <QQuickItem>
#include "qlcvglobal.h"
#include "qmat.h"

class Q_LCV_EXPORT QMatDisplay : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QMat* output       READ output       NOTIFY outputChanged)
    Q_PROPERTY(bool  linearFilter READ linearFilter WRITE setLinearFilter NOTIFY linearFilterChanged)

public:
    explicit QMatDisplay(QQuickItem *parent = 0);
    QMatDisplay(QMat* output, QQuickItem *parent = 0);
    virtual ~QMatDisplay();

    QMat* output();

    bool linearFilter() const;
    void setLinearFilter(bool filter);

signals:
    void outputChanged();
    void linearFilterChanged();

protected:
    void setOutput(QMat* mat);
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData);
    
private:
    QMat* m_output;
    bool  m_linearFilter;

};

inline QMat *QMatDisplay::output(){
    return m_output;
}

inline void QMatDisplay::setOutput(QMat *mat){
    m_output = mat;
    emit outputChanged();
}

inline bool QMatDisplay::linearFilter() const{
    return m_linearFilter;
}

inline void QMatDisplay::setLinearFilter(bool filter){
    if ( filter != m_linearFilter ){
        m_linearFilter = filter;
        emit linearFilterChanged();
        update();
    }
}

#endif // QMATDISPLAY_HPP
