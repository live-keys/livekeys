/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef LVVALUEHISTORY_H
#define LVVALUEHISTORY_H

#include <QQuickItem>
#include <QSGSimpleTextureNode>
#include <list>
#include <limits.h>

class QPainter;
class QOpenGLPaintDevice;
class QOpenGLFramebufferObject;
class QOpenGLFunctions;

namespace lv{

/// \private
class DrawValueHistoryNode : public QObject, public QSGSimpleTextureNode{

    Q_OBJECT

public:
    DrawValueHistoryNode(QQuickWindow* window);
    ~DrawValueHistoryNode();

    void render(const std::list<double>& values, double minVal, double maxVal, const QColor &color);

private:
    QOpenGLFramebufferObject *m_fbo;
    QSGTexture               *m_texture;
    QQuickWindow             *m_window;
    QPainter                 *m_painter;
    QOpenGLPaintDevice       *m_paintDevice;
    QOpenGLFunctions         *m_glFunctions;
};

/// \private
class ValueHistory : public QQuickItem{

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
    ValueHistory(QQuickItem* parent = 0);
    ~ValueHistory();

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
    double             m_maxDisplayValue;
    double             m_minDisplayValue;
    int                m_maxHistory;
    QColor             m_displayColor;
    std::list<double>* m_values;
};

inline double ValueHistory::currentValue() const{
    return m_values->size() > 0 ? m_values->back() : -1;
}

inline double ValueHistory::maxDisplayValue() const{
    return m_maxDisplayValue;
}

inline double ValueHistory::minDisplayValue() const{
    return m_minDisplayValue;
}

inline int ValueHistory::maxHistory() const{
    return m_maxHistory;
}

inline const QColor &ValueHistory::renderColor() const{
    return m_displayColor;
}

inline void ValueHistory::setCurrentValue(double currentValue){
    m_values->push_back(currentValue);
    if ( static_cast<int>(m_values->size()) > m_maxHistory )
        m_values->pop_front();
    emit currentValueChanged();

    update();
}

inline void ValueHistory::setMaxDisplayValue(double maxDisplayValue){
    if ( m_maxDisplayValue != maxDisplayValue ){
        m_maxDisplayValue = maxDisplayValue;
        emit maxDisplayValueChanged();
        update();
    }
}

inline void ValueHistory::setMinDisplayValue(double minDisplayValue){
    if ( m_minDisplayValue != minDisplayValue ){
        m_minDisplayValue = minDisplayValue;
        emit minDisplayValueChanged();
        update();
    }
}

inline void ValueHistory::setMaxHistory(int maxHistory){
    if ( m_maxHistory == maxHistory || maxHistory < 1 )
        return;

    m_maxHistory = maxHistory;
    while ( m_maxHistory > static_cast<int>(m_values->size()) ){
        m_values->pop_front();
    }

    emit maxHistoryChanged();
    update();
}

inline void ValueHistory::setRenderColor(const QColor &color){
    if ( color == m_displayColor )
        return;

    m_displayColor = color;
    emit renderColorChanged();

    update();
}

} // namespace

#endif // LVVALUEHISTORY_H
