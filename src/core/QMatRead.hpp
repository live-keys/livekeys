#ifndef QMATVALUES_HPP
#define QMATVALUES_HPP

#include <QQuickItem>
#include <QSGSimpleTextureNode>
#include "QMat.hpp"

class QPainter;
class QOpenGLPaintDevice;
class QOpenGLFramebufferObject;
class QMatReadNode : public QObject, public QSGSimpleTextureNode{

    Q_OBJECT

public:
    QMatReadNode(QQuickWindow* window);
    ~QMatReadNode();

    void render(QMat* image, const QFont& font, const QColor& color, int numberWidth = 5, bool equalAspectRatio = false);

private:
    QOpenGLFramebufferObject *m_fbo;
    QSGTexture               *m_texture;
    QQuickWindow             *m_window;
    QPainter                 *m_painter;
    QOpenGLPaintDevice       *m_paintDevice;

};

class QMatRead : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QMat* input READ inputMat WRITE setInputMat NOTIFY inputChanged)

public:
    explicit QMatRead(QQuickItem *parent = 0);
    ~QMatRead();

    QMat* inputMat();
    void setInputMat(QMat* mat);

signals:
    void inputChanged();

public slots:

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *);

private:
    QMatRead(const QMatRead& other);
    QMatRead& operator= (const QMatRead& other);

    QMat*       m_input;
};

inline QMat *QMatRead::inputMat(){
    return m_input;
}

inline void QMatRead::setInputMat(QMat *mat){
    m_input = mat;
    emit inputChanged();
    update();
}

#endif // QMATVALUES_HPP
