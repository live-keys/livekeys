/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/

#ifndef QMATEMPTY_HPP
#define QMATEMPTY_HPP

#include "qmatdisplay.h"

class QMatEmpty : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QSize      matSize  READ matSize  WRITE setMatSize  NOTIFY matSizeChanged)
    Q_PROPERTY(QColor     color    READ color    WRITE setColor    NOTIFY colorChanged)
    Q_PROPERTY(int        channels READ channels WRITE setChannels NOTIFY channelsChanged)
    Q_PROPERTY(QMat::Type type     READ type     WRITE setType     NOTIFY typeChanged)

public:
    explicit QMatEmpty(QQuickItem *parent = 0);
    ~QMatEmpty();

    const QSize& matSize() const;
    void setMatSize(const QSize& size);

    const QColor& color() const;
    void setColor(const QColor& color);

    int channels() const;
    void setChannels(int channels);

    QMat::Type type() const;
    void setType(QMat::Type type);

    void cacheScalarColor();

signals:
    void matSizeChanged();
    void colorChanged();
    void channelsChanged();
    void typeChanged();

private:
    QSize      m_matSize;
    QColor     m_color;
    int        m_channels;
    QMat::Type m_type;
    cv::Scalar m_colorScalar;

};

inline const QSize &QMatEmpty::matSize() const{
    return m_matSize;
}

inline void QMatEmpty::setMatSize(const QSize &size){
    if ( m_matSize != size ){
        m_matSize = size;
        setImplicitSize(size.width(), size.height());
        emit matSizeChanged();
        output()->cvMat()->create(cv::Size(m_matSize.width(), m_matSize.height()), CV_MAKETYPE(m_type, m_channels));
        output()->cvMat()->setTo(m_colorScalar);
        emit outputChanged();
        update();
    }
}

inline const QColor &QMatEmpty::color() const{
    return m_color;
}

inline void QMatEmpty::setColor(const QColor &color){
    if ( m_color != color ){
        m_color = color;
        cacheScalarColor();
        output()->cvMat()->setTo(m_colorScalar);
        emit colorChanged();
        emit outputChanged();
        update();
    }
}

inline int QMatEmpty::channels() const{
    return m_channels;
}

inline void QMatEmpty::setChannels(int channels){
    if ( m_channels != channels ){
        m_channels = channels;

        cacheScalarColor();
        output()->cvMat()->create(cv::Size(m_matSize.width(), m_matSize.height()), CV_MAKETYPE(m_type, m_channels));
        output()->cvMat()->setTo(m_colorScalar);

        emit channelsChanged();
        emit outputChanged();
        update();
    }
}

inline QMat::Type QMatEmpty::type() const{
    return m_type;
}

inline void QMatEmpty::setType(QMat::Type type){
    if ( m_type == type ){
        m_type = type;
        emit typeChanged();
        output()->cvMat()->create(cv::Size(m_matSize.width(), m_matSize.height()), CV_MAKETYPE(m_type, m_channels));
        output()->cvMat()->setTo(m_colorScalar);
        emit outputChanged();
        update();
    }
}

inline void QMatEmpty::cacheScalarColor(){
    switch ( m_channels ){
    case 1 : m_colorScalar = cv::Scalar(m_color.red());break;
    case 2 : m_colorScalar = cv::Scalar(m_color.red(),  m_color.green()); break;
    case 3 : m_colorScalar = cv::Scalar(m_color.blue(), m_color.green(), m_color.red()); break;
    case 4 : m_colorScalar = cv::Scalar(m_color.blue(), m_color.green(), m_color.red(), m_color.alpha()); break;
    }
}

#endif // QMATEMPTY_HPP
