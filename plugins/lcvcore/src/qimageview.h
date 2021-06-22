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
