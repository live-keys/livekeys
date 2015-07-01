#ifndef QDRAWHISTOGRAM_HPP
#define QDRAWHISTOGRAM_HPP

#include <QQuickItem>
#include <QSGSimpleTextureNode>

class QPainter;
class QOpenGLPaintDevice;
class QOpenGLFramebufferObject;
class QDrawHistogramNode : public QObject, public QSGSimpleTextureNode{

    Q_OBJECT

public:
    QDrawHistogramNode(QQuickWindow* window);
    ~QDrawHistogramNode();

    void render(const QVariantList& values, const QVariantList& colors, qreal maxValue);

private:
    static void renderSingleList(QPainter* painter,
        const QSize &size,
        const QVariantList& values,
        const QColor &color,
        qreal maxValue);

    QOpenGLFramebufferObject *m_fbo;
    QSGTexture               *m_texture;
    QQuickWindow             *m_window;
    QPainter                 *m_painter;
    QOpenGLPaintDevice       *m_paintDevice;
};


class QDrawHistogram : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QVariantList values   READ values   WRITE setValues   NOTIFY valuesChanged)
    Q_PROPERTY(QVariantList colors   READ colors   WRITE setColors   NOTIFY colorsChanged)
    Q_PROPERTY(qreal        maxValue READ maxValue WRITE setMaxValue NOTIFY maxValueChanged)

public:
    QDrawHistogram(QQuickItem* parent = 0);
    ~QDrawHistogram();

    const QVariantList& values() const;
    const QVariantList& colors() const;
    qreal maxValue() const;

    void setValues(const QVariantList& arg);
    void setColors(const QVariantList& arg);
    void setMaxValue(qreal arg);

signals:
    void valuesChanged();
    void colorsChanged();
    void maxValueChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *nodeData);

private:
    QDrawHistogram(const QDrawHistogram& other);
    QDrawHistogram& operator = (const QDrawHistogram& other);

    QVariantList m_values;
    QVariantList m_colors;
    qreal        m_maxValue;
};

inline const QVariantList& QDrawHistogram::values() const{
    return m_values;
}

inline const QVariantList& QDrawHistogram::colors() const{
    return m_colors;
}

inline void QDrawHistogram::setValues(const QVariantList &arg){
    if (m_values == arg)
        return;

    m_values = arg;
    emit valuesChanged();
    update();
}

inline qreal QDrawHistogram::maxValue() const{
    return m_maxValue;
}

inline void QDrawHistogram::setColors(const QVariantList &arg){
    if (m_colors == arg)
        return;

    m_colors = arg;
    emit colorsChanged();
    update();
}

inline void QDrawHistogram::setMaxValue(qreal arg){
    if (m_maxValue == arg)
        return;

    m_maxValue = arg;
    emit maxValueChanged();
    update();
}

#endif // QDRAWHISTOGRAM_HPP
