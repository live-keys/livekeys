/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#ifndef QMATREAD_H
#define QMATREAD_H

#include <QQuickItem>
#include <QSGSimpleTextureNode>
#include "qmat.h"

class QPainter;
class QOpenGLPaintDevice;
class QOpenGLFramebufferObject;
class QOpenGLFunctions;
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
    QOpenGLFunctions         *m_glFunctions;

};

/// \private
class QMatRead : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QMat*  input       READ inputMat    WRITE setInputMat    NOTIFY inputChanged)
    Q_PROPERTY(QFont  font        READ font        WRITE setFont        NOTIFY fontChanged)
    Q_PROPERTY(QColor color       READ color       WRITE setColor       NOTIFY colorChanged)
    Q_PROPERTY(int    numberWidth READ numberWidth WRITE setNumberWidth NOTIFY numberWidthChanged)
    Q_PROPERTY(bool   squareCell  READ squareCell  WRITE setSquareCell  NOTIFY squareCellChanged)

public:
    explicit QMatRead(QQuickItem *parent = 0);
    ~QMatRead();

    QMat* inputMat();
    void setInputMat(QMat* mat);

    const QFont& font() const;
    const QColor& color() const;
    int numberWidth() const;
    bool squareCell() const;

    void setFont(const QFont &arg);
    void setColor(const QColor& arg);
    void setNumberWidth(int arg);
    void setSquareCell(bool arg);

    void calculateImplicitSize();

signals:
    void inputChanged();
    void fontChanged();
    void colorChanged();
    void numberWidthChanged();
    void squareCellChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *nodeData);

private:
    QMatRead(const QMatRead& other);
    QMatRead& operator= (const QMatRead& other);

    QMat*  m_input;
    QFont  m_font;
    QColor m_color;
    int    m_numberWidth;
    bool   m_squareCell;
};

inline const QFont& QMatRead::font() const{
    return m_font;
}

inline const QColor& QMatRead::color() const{
    return m_color;
}

inline int QMatRead::numberWidth() const{
    return m_numberWidth;
}

inline bool QMatRead::squareCell() const{
    return m_squareCell;
}

inline void QMatRead::setFont(const QFont& arg){
    if (m_font != arg) {
        m_font = arg;
        emit fontChanged();
        calculateImplicitSize();
        update();
    }
}

inline void QMatRead::setColor(const QColor& arg){
    if (m_color != arg) {
        m_color = arg;
        emit colorChanged();
        update();
    }
}

inline void QMatRead::setNumberWidth(int arg){
    if (m_numberWidth != arg) {
        m_numberWidth = arg;
        emit numberWidthChanged();
        calculateImplicitSize();
        update();
    }
}

inline void QMatRead::setSquareCell(bool arg){
    if (m_squareCell != arg) {
        m_squareCell = arg;
        emit squareCellChanged();
        calculateImplicitSize();
        update();
    }
}

inline QMat *QMatRead::inputMat(){
    return m_input;
}

inline void QMatRead::setInputMat(QMat *mat){
    if ( mat == 0 )
        return;

    if ( mat != m_input ){
        m_input = mat;
        calculateImplicitSize();
    } else {
        m_input = mat;
        if ( implicitHeight() == 0 || implicitWidth() == 0){
            calculateImplicitSize();
        }
    }
    emit inputChanged();
    update();
}

#endif // QMATREAD_H
