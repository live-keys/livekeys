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

#ifndef QMATVIEW_H
#define QMATVIEW_H

#include "qmatdisplay.h"

/// \private
class QMatView : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QMat* mat         READ mat          WRITE setMat          NOTIFY matChanged)
    Q_PROPERTY(bool linearFilter READ linearFilter WRITE setLinearFilter NOTIFY linearFilterChanged)

public:
    explicit QMatView(QQuickItem* parent = nullptr);
    ~QMatView();

    QMat* mat();
    void setMat(QMat* mat);

    bool linearFilter() const;
    void setLinearFilter(bool filter);

signals:
    void matChanged(QMat* arg);
    void linearFilterChanged();

protected:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData);

private:
    QMat* m_mat;
    bool  m_linearFilter;
};

inline QMat *QMatView::mat(){
    return m_mat;
}

inline void QMatView::setMat(QMat *arg){
    if ( arg == nullptr )
        return;

    cv::Mat* matData = arg->cvMat();
    if ( static_cast<int>(implicitWidth()) != matData->cols || static_cast<int>(implicitHeight()) != matData->rows ){
        setImplicitWidth(matData->cols);
        setImplicitHeight(matData->rows);
    }

    if ( m_mat ){
        lv::Shared::unref(m_mat);
    }

    m_mat = arg;
    lv::Shared::ref(m_mat);

    emit matChanged(arg);
    update();
}

inline bool QMatView::linearFilter() const{
    return m_linearFilter;
}

inline void QMatView::setLinearFilter(bool filter){
    if ( filter != m_linearFilter ){
        m_linearFilter = filter;
        emit linearFilterChanged();
        update();
    }
}

#endif // QMATVIEW_H
