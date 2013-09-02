#ifndef QMATDISPLAY_HPP
#define QMATDISPLAY_HPP

#include <QQuickItem>
#include "QMat.hpp"

class QMatDisplay : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QMat* output READ output WRITE setOutput NOTIFY outChanged)

public:
    explicit QMatDisplay(QQuickItem *parent = 0);
    QMatDisplay(QMat* output, QQuickItem *parent = 0);
    virtual ~QMatDisplay();

    QMat* output();
    void setOutput(QMat* mat);

signals:
    void outChanged();

protected:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *);
    
private:
    QMat* m_output;

};

inline QMat *QMatDisplay::output(){
    return m_output;
}

inline void QMatDisplay::setOutput(QMat *mat){
    m_output = mat;
	emit outChanged();
}

#endif // QMATDISPLAY_HPP
