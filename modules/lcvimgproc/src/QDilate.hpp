#ifndef QDILATE_HPP
#define QDILATE_HPP

#include "QMatFilter.hpp"

class QDilate : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(QMat*  kernel      READ kernel      WRITE setKernel      NOTIFY kernelChanged)
    Q_PROPERTY(QPoint anchor      READ anchor      WRITE setAnchor      NOTIFY anchorChanged)
    Q_PROPERTY(int    iterations  READ iterations  WRITE setIterations  NOTIFY iterationsChanged)
    Q_PROPERTY(int    borderType  READ borderType  WRITE setBorderType  NOTIFY borderTypeChanged)
    Q_PROPERTY(QColor borderValue READ borderValue WRITE setBorderValue NOTIFY borderValueChanged)

public:
    QDilate(QQuickItem* parent = 0);
    ~QDilate();

signals:
    void kernelChanged();
    void anchorChanged();
    void iterationsChanged();
    void borderTypeChanged();
    void borderValueChanged();

public:
	QMat* kernel() const;
	void setKernel(QMat* kernel);

	const QPoint& anchor() const;
	void setAnchor(const QPoint& anchor);

	int iterations() const;
	void setIterations(int iterations);

	int borderType() const;
	void setBorderType(int borderType);

	const QColor& borderValue() const;
	void setBorderValue(const QColor& borderValue);

    void transform(cv::Mat& in, cv::Mat& out);

private:
    QMat*      m_kernel;
    QPoint     m_anchor;
    cv::Point  m_anchorCv;
    int        m_iterations;
    int        m_borderType;
    QColor     m_borderValue;
};

inline const QColor& QDilate::borderValue() const{
	return m_borderValue;
}

inline void QDilate::setBorderValue(const QColor& borderValue){
	if (m_borderValue != borderValue){
		m_borderValue = borderValue;
		emit borderValueChanged();
    }
}

inline int QDilate::borderType() const{
	return m_borderType;
}

inline void QDilate::setBorderType(int borderType){
	if (m_borderType != borderType){
		m_borderType = borderType;
		emit borderTypeChanged();
	}
}

inline int QDilate::iterations() const{
	return m_iterations;
}

inline void QDilate::setIterations(int iterations){
	if (m_iterations != iterations){
		m_iterations = iterations;
		emit iterationsChanged();
	}
}

inline const QPoint& QDilate::anchor() const{
	return m_anchor;
}

inline void QDilate::setAnchor(const QPoint& anchor){
	if (m_anchor != anchor){
        m_anchor   = anchor;
        m_anchorCv = cv::Point(anchor.x(), anchor.y());
		emit anchorChanged();
	}
}

inline QMat* QDilate::kernel() const{
	return m_kernel;
}

inline void QDilate::setKernel(QMat*kernel){
	if (m_kernel != kernel){
		m_kernel = kernel;
		emit kernelChanged();
	}
}


#endif // QDILATE_HPP
