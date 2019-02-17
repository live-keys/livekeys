#ifndef QVALUEHISTORY_H
#define QVALUEHISTORY_H

#include <QQuickItem>
#include <QLinkedList>
#include <QSGSimpleTextureNode>
#include <limits.h>

class QPainter;
class QOpenGLPaintDevice;
class QOpenGLFramebufferObject;
class QOpenGLFunctions;

/// \private
class QDrawValueHistoryNode : public QObject, public QSGSimpleTextureNode{

    Q_OBJECT

public:
    QDrawValueHistoryNode(QQuickWindow* window);
    ~QDrawValueHistoryNode();

    void render(const QLinkedList<double>& values, double minVal, double maxVal, const QColor &color);

private:
    QOpenGLFramebufferObject *m_fbo;
    QSGTexture               *m_texture;
    QQuickWindow             *m_window;
    QPainter                 *m_painter;
    QOpenGLPaintDevice       *m_paintDevice;
    QOpenGLFunctions         *m_glFunctions;
};

/// \private
class QValueHistory : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(double currentValue    READ currentValue    WRITE setCurrentValue    NOTIFY currentValueChanged)
    Q_PROPERTY(double maxDisplayValue READ maxDisplayValue WRITE setMaxDisplayValue NOTIFY maxDisplayValueChanged)
    Q_PROPERTY(double minDisplayValue READ minDisplayValue WRITE setMinDisplayValue NOTIFY minDisplayValueChanged)
    Q_PROPERTY(int    maxHistory      READ maxHistory      WRITE setMaxHistory      NOTIFY maxHistoryChanged)
    Q_PROPERTY(QColor renderColor     READ renderColor     WRITE setRenderColor     NOTIFY renderColorChanged)
    Q_ENUMS(DisplayHeight)

public:
    enum DisplayHeight{
        Auto = INT_MIN
    };

public:
    QValueHistory(QQuickItem* parent = 0);
    ~QValueHistory();

    double currentValue() const;
    double maxDisplayValue() const;
    double minDisplayValue() const;
    int maxHistory() const;
    const QColor& renderColor() const;

    void setCurrentValue(double currentValue);
    void setMaxDisplayValue(double maxDisplayValue);
    void setMinDisplayValue(double minDisplayValue);
    void setMaxHistory(int maxHistory);
    void setRenderColor(const QColor& renderColor);

signals:
    void currentValueChanged();
    void maxDisplayValueChanged();
    void minDisplayValueChanged();
    void maxHistoryChanged();
    void renderColorChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *nodeData);

private:
    double m_maxDisplayValue;
    double m_minDisplayValue;
    int    m_maxHistory;
    QColor m_displayColor;
    QLinkedList<double> m_values;
};

inline double QValueHistory::currentValue() const{
    return m_values.size() > 0 ? m_values.last() : -1;
}

inline double QValueHistory::maxDisplayValue() const{
    return m_maxDisplayValue;
}

inline double QValueHistory::minDisplayValue() const{
    return m_minDisplayValue;
}

inline int QValueHistory::maxHistory() const{
    return m_maxHistory;
}

inline const QColor &QValueHistory::renderColor() const{
    return m_displayColor;
}

inline void QValueHistory::setCurrentValue(double currentValue){
    m_values.append(currentValue);
    if ( m_values.size() > m_maxHistory )
        m_values.pop_front();
    emit currentValueChanged();

    update();
}

inline void QValueHistory::setMaxDisplayValue(double maxDisplayValue){
    if ( m_maxDisplayValue != maxDisplayValue ){
        m_maxDisplayValue = maxDisplayValue;
        emit maxDisplayValueChanged();
        update();
    }
}

inline void QValueHistory::setMinDisplayValue(double minDisplayValue){
    if ( m_minDisplayValue != minDisplayValue ){
        m_minDisplayValue = minDisplayValue;
        emit minDisplayValueChanged();
        update();
    }
}

inline void QValueHistory::setMaxHistory(int maxHistory){
    if ( m_maxHistory == maxHistory || maxHistory < 1 )
        return;

    m_maxHistory = maxHistory;
    while ( m_maxHistory > m_values.size() ){
        m_values.pop_front();
    }

    emit maxHistoryChanged();
    update();
}

inline void QValueHistory::setRenderColor(const QColor &color){
    if ( color == m_displayColor )
        return;

    m_displayColor = color;
    emit renderColorChanged();

    update();
}

#endif // QVALUEHISTORY
