#ifndef QBLUR_HPP
#define QBLUR_HPP

#include <QQuickItem>
#include "QMatFilter.hpp"
//void blur(InputArray src, OutputArray dst, Size ksize, QPoint anchor=QPoint(-1,-1), int borderType=BORDER_DEFAULT )

class QBlur : public QMatFilter{

    Q_OBJECT

public:
    explicit QBlur(QQuickItem *parent = 0);
    ~QBlur();

    virtual void transform(cv::Mat &in, cv::Mat &out);

signals:
    void ksizeChanged();
    void borderTypeChanged();
    void anchorChanged();

public slots:
	const QSize& ksize() const;
    void setKsize(const QSize& ksize);

    const QPoint& anchor() const;
    void setAnchor(const QPoint& anchor);

    int borderType() const;
    void setBorderType(int borderType);

private:
    int    m_borderType;
    QSize  m_ksize;
    QPoint m_anchor;

};

inline int QBlur::borderType() const{
	return m_borderType;
}

inline void QBlur::setBorderType(int borderType){
	if (m_borderType != borderType){
		m_borderType = borderType;
		emit borderTypeChanged();
	}
}

inline const QPoint& QBlur::anchor() const{
	return m_anchor;
}

inline void QBlur::setAnchor(const QPoint& anchor){
	if (m_anchor != anchor){
		m_anchor = anchor;
		emit anchorChanged();
	}
}

inline const QSize& QBlur::ksize() const{
	return m_ksize;
}

inline void QBlur::setKsize(const QSize& ksize){
	if (m_ksize != ksize){
		m_ksize = ksize;
		emit ksizeChanged();
	}
}


#endif // QBLUR_HPP
