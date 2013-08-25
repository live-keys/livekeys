#ifndef QMATDISPLAY_HPP
#define QMATDISPLAY_HPP

#include <QQuickItem>
#include "QMat.hpp"

class QMatDisplay : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QMat* out READ out WRITE setOut NOTIFY outChanged)

public:
    explicit QMatDisplay(QQuickItem *parent = 0);
    virtual ~QMatDisplay();

    QMat* out();
    void setOut(QMat* mat);

signals:
    void outChanged();

protected:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *);
    
private:
    QMat* m_out;

};

inline QMat *QMatDisplay::out(){
    return m_out;
}

inline void QMatDisplay::setOut(QMat *mat){
	m_out = mat;
	emit outChanged();
}

#endif // QMATDISPLAY_HPP
