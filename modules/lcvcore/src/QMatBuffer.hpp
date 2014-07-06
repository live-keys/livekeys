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

#ifndef QMATBUFFER_HPP
#define QMATBUFFER_HPP

#include <QQuickItem>
#include "QLCVGlobal.hpp"
#include "QMatDisplay.hpp"

class QMatBuffer : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY( QMat* input READ inputMat WRITE setInputMat NOTIFY inputChanged )

public:
    explicit QMatBuffer(QQuickItem *parent = 0);
    ~QMatBuffer();

    void setInputMat(QMat* input);
    QMat* inputMat();

signals:
    void inputChanged();

private:
    QMat* m_matBuffer;
    QMat* m_matCurrent;

};


inline void QMatBuffer::setInputMat(QMat *input){
    m_matCurrent->cvMat()->copyTo(*m_matBuffer->cvMat());
    input->cvMat()->copyTo(*m_matCurrent->cvMat());
    emit inputChanged();
    setOutput(m_matBuffer);
}

inline QMat *QMatBuffer::inputMat(){
    return m_matCurrent;
}

#endif // QMATBUFFER_HPP
