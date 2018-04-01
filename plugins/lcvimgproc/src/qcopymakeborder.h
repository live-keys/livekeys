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

#ifndef QCOPYMAKEBORDER_H
#define QCOPYMAKEBORDER_H

#include "qmatfilter.h"
#include "opencv2/imgproc.hpp"

class QCopyMakeBorder : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(int    top        READ top        WRITE setTop        NOTIFY topChanged)
    Q_PROPERTY(int    bottom     READ bottom     WRITE setBottom     NOTIFY bottomChanged)
    Q_PROPERTY(int    left       READ left       WRITE setLeft       NOTIFY leftChanged)
    Q_PROPERTY(int    right      READ right      WRITE setRight      NOTIFY rightChanged)
    Q_PROPERTY(int    borderType READ borderType WRITE setBorderType NOTIFY borderTypeChanged)
    Q_PROPERTY(QColor color      READ color      WRITE setColor      NOTIFY colorChanged)

    Q_ENUMS(BorderType)

public:
    enum BorderType{
        BORDER_REPLICATE   = cv::BORDER_REPLICATE,
        BORDER_CONSTANT    = cv::BORDER_CONSTANT,
        BORDER_REFLECT     = cv::BORDER_REFLECT,
        BORDER_WRAP        = cv::BORDER_WRAP,
        BORDER_REFLECT_101 = cv::BORDER_REFLECT_101,
        BORDER_TRANSPARENT = cv::BORDER_TRANSPARENT,
        BORDER_DEFAULT     = cv::BORDER_REFLECT_101,
        BORDER_ISOLATED    = cv::BORDER_ISOLATED
    };

public:
    explicit QCopyMakeBorder(QQuickItem *parent = 0);
    ~QCopyMakeBorder();

    virtual void transform(const cv::Mat& in, cv::Mat& out);

signals:
    void topChanged();
    void bottomChanged();
    void leftChanged();
    void rightChanged();
    void borderTypeChanged();
    void colorChanged();

public:
    int top() const;
    void setTop(int top);

    int bottom() const;
    void setBottom(int bottom);

    int left() const;
    void setLeft(int left);

    int right() const;
    void setRight(int right);

    int borderType() const;
    void setBorderType(int borderType);

	const QColor& color() const;
    void setColor(const QColor& color);

private:
    int m_top;
    int m_left;
    int m_right;
    int m_bottom;
    int m_borderType;
    QColor m_color;
};

inline const QColor& QCopyMakeBorder::color() const{
	return m_color;
}

inline void QCopyMakeBorder::setColor(const QColor& color){
	if (m_color != color){
		m_color = color;
		emit colorChanged();
	}
}

inline int QCopyMakeBorder::borderType() const{
	return m_borderType;
}

inline void QCopyMakeBorder::setBorderType(int borderType){
	if (m_borderType != borderType){
		m_borderType = borderType;
		emit borderTypeChanged();
	}
}

inline int QCopyMakeBorder::right() const{
	return m_right;
}

inline void QCopyMakeBorder::setRight(int right){
	if (m_right != right){
		m_right = right;
		emit rightChanged();
	}
}

inline int QCopyMakeBorder::left() const{
	return m_left;
}

inline void QCopyMakeBorder::setLeft(int left){
	if (m_left != left){
		m_left = left;
		emit leftChanged();
	}
}

inline int QCopyMakeBorder::bottom() const{
	return m_bottom;
}

inline void QCopyMakeBorder::setBottom(int bottom){
	if (m_bottom != bottom){
		m_bottom = bottom;
		emit bottomChanged();
	}
}

inline int QCopyMakeBorder::top() const{
	return m_top;
}

inline void QCopyMakeBorder::setTop(int top){
	if (m_top != top){
		m_top = top;
		emit topChanged();
	}
}


#endif // QCOPYMAKEBORDER_H
