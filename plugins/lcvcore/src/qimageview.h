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

#ifndef QIMAGEVIEW_H
#define QIMAGEVIEW_H

#include <QQuickItem>
#include "qmat.h"

/// \private
class QImageView : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QMat* image       READ image        WRITE setImage        NOTIFY imageChanged)
    Q_PROPERTY(bool linearFilter READ linearFilter WRITE setLinearFilter NOTIFY linearFilterChanged)

public:
    explicit QImageView(QQuickItem* parent = nullptr);
    ~QImageView();

    QMat* image();
    void setImage(QMat* image);

    bool linearFilter() const;
    void setLinearFilter(bool filter);

signals:
    void imageChanged();
    void linearFilterChanged();

protected:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData);

private:
    QMat* m_mat;
    bool  m_linearFilter;
};

inline QMat *QImageView::image(){
    return m_mat;
}



inline bool QImageView::linearFilter() const{
    return m_linearFilter;
}

inline void QImageView::setLinearFilter(bool filter){
    if ( filter != m_linearFilter ){
        m_linearFilter = filter;
        emit linearFilterChanged();
        update();
    }
}

#endif // QIMAGEVIEW_H
