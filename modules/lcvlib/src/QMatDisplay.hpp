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

#ifndef QMATDISPLAY_HPP
#define QMATDISPLAY_HPP

#include <QQuickItem>
#include "QLCVGlobal.hpp"
#include "QMat.hpp"

class Q_LCV_EXPORT QMatDisplay : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QMat* output       READ output       WRITE setOutput       NOTIFY outChanged)
    Q_PROPERTY(bool  linearFilter READ linearFilter WRITE setLinearFilter NOTIFY linearFilterChanged)

public:
    explicit QMatDisplay(QQuickItem *parent = 0);
    QMatDisplay(QMat* output, QQuickItem *parent = 0);
    virtual ~QMatDisplay();

    QMat* output();
    void setOutput(QMat* mat);

    bool linearFilter() const;
    void setLinearFilter(bool filter);

signals:
    void outChanged();
    void linearFilterChanged();

protected:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *);
    
private:
    QMat* m_output;
    bool  m_linearFilter;

};

inline QMat *QMatDisplay::output(){
    return m_output;
}

inline void QMatDisplay::setOutput(QMat *mat){
    m_output = mat;
    emit outChanged();
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
