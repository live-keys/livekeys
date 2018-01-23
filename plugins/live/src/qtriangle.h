#ifndef QTRIANGLE_H
#define QTRIANGLE_H

#include <QQuickPaintedItem>

class QTriangle : public QQuickPaintedItem{

    Q_OBJECT
    Q_PROPERTY(int rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(QColor color READ color    WRITE setColor    NOTIFY colorChanged)
    Q_ENUMS(Rotation)

public:
    enum Rotation{
        Top = 0,
        Right,
        Bottom,
        Left
    };

public:
    explicit QTriangle(QQuickItem *parent = nullptr);

    const QColor& color() const;
    void setColor(const QColor color);

    int rotation() const;
    void setRotation(int rotation);

    void paint(QPainter *painter);

signals:
    void rotationChanged();
    void colorChanged();

private:
    void updatePath();

private:
    int     m_rotation;
    QColor  m_color;
    QRect   m_lastBoundingRect;
    QPointF m_points[3];
};

inline const QColor& QTriangle::color() const{
    return m_color;
}

inline void QTriangle::setColor(const QColor color){
    if ( m_color != color ){
        m_color = color;
        update();
        emit colorChanged();
    }
}

inline int QTriangle::rotation() const{
     return m_rotation;
}

inline void QTriangle::setRotation(int rotation){
     if ( m_rotation != rotation ){
         m_rotation = rotation;
         updatePath();
         update();
         emit rotationChanged();
    }
}

#endif // QTRIANGLE_H
