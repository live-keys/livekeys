#ifndef LVLINEGRID_H
#define LVLINEGRID_H

#include <QQuickPaintedItem>

namespace lv{

class LineGrid : public QQuickPaintedItem{

    Q_OBJECT
    Q_PROPERTY(double colSpacing READ colSpacing WRITE setColSpacing NOTIFY colSpacingChanged)
    Q_PROPERTY(double rowSpacing READ rowSpacing WRITE setRowSpacing NOTIFY rowSpacingChanged)
    Q_PROPERTY(double thickness  READ thickness  WRITE setThickness  NOTIFY thicknessChanged)
    Q_PROPERTY(QColor color      READ color      WRITE setColor      NOTIFY colorChanged)

public:
    LineGrid(QQuickItem* parent = nullptr);

    double colSpacing() const;
    double rowSpacing() const;
    double thickness() const;
    QColor color() const;

    void paint(QPainter *painter);

public slots:
    void setColSpacing(double colSpacing);
    void setRowSpacing(double rowSpacing);
    void setThickness(double thickness);
    void setColor(QColor color);

signals:
    void colSpacingChanged();
    void rowSpacingChanged();
    void thicknessChanged();
    void colorChanged();

private:
    double m_colSpacing;
    double m_rowSpacing;
    double m_thickness;
    QColor m_color;
};

inline double LineGrid::colSpacing() const{
    return m_colSpacing;
}

inline double LineGrid::rowSpacing() const{
    return m_rowSpacing;
}

inline double LineGrid::thickness() const{
    return m_thickness;
}

inline QColor LineGrid::color() const{
    return m_color;
}

inline void LineGrid::setColSpacing(double colSpacing){
    if (qFuzzyCompare(m_colSpacing, colSpacing))
        return;

    m_colSpacing = colSpacing;
    emit colSpacingChanged();
    update();
}

inline void LineGrid::setRowSpacing(double rowSpacing){
    if (qFuzzyCompare(m_rowSpacing, rowSpacing))
        return;

    m_rowSpacing = rowSpacing;
    emit rowSpacingChanged();
    update();
}

inline void LineGrid::setThickness(double thickness){
    if (qFuzzyCompare(m_thickness, thickness))
        return;

    m_thickness = thickness;
    emit thicknessChanged();
    update();
}

inline void LineGrid::setColor(QColor color){
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged();
    update();
}

}// namespace

#endif // LINEGRID_H
