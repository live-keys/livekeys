#ifndef QMATVALUES_HPP
#define QMATVALUES_HPP

#include <QQuickItem>
#include <QSGSimpleTextureNode>

class QMatValues : public QQuickItem{

    Q_OBJECT

public:
    explicit QMatValues(QQuickItem *parent = 0);
    ~QMatValues();



signals:

public slots:

protected:
    QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *);

private:
    QSGTexture* m_texture;
    QImage      m_surface;

};

#endif // QMATVALUES_HPP
