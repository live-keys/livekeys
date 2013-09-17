#ifndef QMATDISPLAY_HPP
#define QMATDISPLAY_HPP

#include <QQuickItem>
#include "QMat.hpp"

class QMatDisplay : public QQuickItem{

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
