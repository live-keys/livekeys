#ifndef QCURSORSHAPE_HPP
#define QCURSORSHAPE_HPP

#include <QQuickItem>
#include <QCursor>

class QCursorShape : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(Qt::CursorShape cursorShape READ cursorShape WRITE setCursorShape NOTIFY cursorShapeChanged)

public:
    explicit QCursorShape(QQuickItem *parent = 0);

    Qt::CursorShape cursorShape() const;
    Q_INVOKABLE void setCursorShape(Qt::CursorShape cursorShape);

signals:
    void cursorShapeChanged();

private:
    int m_currentShape;
    
};

inline Qt::CursorShape QCursorShape::cursorShape() const{
  return cursor().shape();
}

inline void QCursorShape::setCursorShape(Qt::CursorShape cursorShape){
    if ( m_currentShape == (int)cursorShape )
        return;
    setCursor(cursorShape);
    emit cursorShapeChanged();
    m_currentShape = cursorShape;
}


#endif // QCURSORSHAPE_HPP
