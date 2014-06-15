#ifndef QFILTER2D_HPP
#define QFILTER2D_HPP

#include "QMatFilter.hpp"

class QFilter2D : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(int    ddepth     READ ddepth     WRITE setDdepth     NOTIFY ddepthChanged)
    Q_PROPERTY(QMat*  kernel     READ kernel     WRITE setKernel     NOTIFY kernelChanged)
    Q_PROPERTY(QPoint anchor     READ anchor     WRITE setAnchor     NOTIFY anchorChanged)
    Q_PROPERTY(double delta      READ delta      WRITE setDelta      NOTIFY deltaChanged)
    Q_PROPERTY(int    borderType READ borderType WRITE setBorderType NOTIFY borderTypeChanged)

public:
    explicit QFilter2D(QQuickItem *parent = 0);
    ~QFilter2D();

    void transform(cv::Mat& in, cv::Mat& out);

signals:
    void ddepthChanged();
    void kernelChanged();
    void anchorChanged();
    void deltaChanged();
    void borderTypeChanged();

public:
	int ddepth() const;
	void setDdepth(int ddepth);

	QMat* kernel() const;
	void setKernel(QMat* kernel);

    const QPoint& anchor() const;
    void setAnchor(const QPoint& anchor);

	double delta() const;
	void setDelta(double delta);

	int borderType() const;
	void setBorderType(int borderType);

private:
    QMat*     m_kernel;
    int       m_ddepth;
    QPoint    m_anchor;
    cv::Point m_anchorCv;
    double    m_delta;
    int       m_borderType;

};

inline int QFilter2D::borderType() const{
	return m_borderType;
}

inline void QFilter2D::setBorderType(int borderType){
	if (m_borderType != borderType){
		m_borderType = borderType;
		emit borderTypeChanged();
	}
}

inline double QFilter2D::delta() const{
	return m_delta;
}

inline void QFilter2D::setDelta(double delta){
	if (m_delta != delta){
		m_delta = delta;
		emit deltaChanged();
	}
}

inline const QPoint& QFilter2D::anchor() const{
	return m_anchor;
}

inline void QFilter2D::setAnchor(const QPoint& anchor){
	if (m_anchor != anchor){
		m_anchor = anchor;
        m_anchorCv = cv::Point(anchor.x(), anchor.y());
		emit anchorChanged();
	}
}

inline QMat* QFilter2D::kernel() const{
	return m_kernel;
}

inline void QFilter2D::setKernel(QMat*kernel){
	if (m_kernel != kernel){
		m_kernel = kernel;
		emit kernelChanged();
	}
}

inline int QFilter2D::ddepth() const{
	return m_ddepth;
}

inline void QFilter2D::setDdepth(int ddepth){
	if (m_ddepth != ddepth){
		m_ddepth = ddepth;
		emit ddepthChanged();
	}
}


#endif // QFILTER2D_HPP
