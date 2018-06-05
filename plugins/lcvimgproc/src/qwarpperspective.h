#ifndef QWARPPERSPECTIVE_H
#define QWARPPERSPECTIVE_H

#include "qmatfilter.h"

class QWarpPerspective : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QMat* output       READ output        WRITE setOutput        NOTIFY outputChanged)
    Q_PROPERTY(QMat* input        READ input         WRITE setInput         NOTIFY inputChanged)
    Q_PROPERTY(QMat* m            READ m             WRITE setM             NOTIFY mChanged)
    Q_PROPERTY(int interpolation  READ interpolation WRITE setInterpolation NOTIFY interpolationChanged)
    Q_PROPERTY(int borderMode     READ borderMode    WRITE setBorderMode    NOTIFY borderModeChanged)
    Q_PROPERTY(QColor borderValue READ borderValue   WRITE setBorderValue   NOTIFY borderValueChanged)

public:
    QWarpPerspective(QQuickItem* parent = nullptr);
    ~QWarpPerspective();

    QMat* m() const;
    int interpolation() const;
    int borderMode() const;
    QColor borderValue() const;
    QMat* input();
    QMat* output();

    void setM(QMat* m);
    void setInterpolation(int interpolation);
    void setBorderMode(int borderMode);
    void setBorderValue(QColor borderValue);
    void setInput(QMat* mat);
    void setOutput(QMat* mat);

    void transform();
    virtual void transform(const cv::Mat &in, cv::Mat &out);

signals:
    void outputChanged();
    void inputChanged();
    void mChanged();
    void interpolationChanged();
    void borderModeChanged();
    void borderValueChanged();

protected:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData) Q_DECL_OVERRIDE;
    void componentComplete() Q_DECL_OVERRIDE;

private:
    QMat*      m_in;
    QMat*      m_output;
    QMat*      m_m;
    int        m_interpolation;
    int        m_borderMode;
    QColor     m_borderValue;
    cv::Scalar m_borderValueInternal;
};

inline QMat *QWarpPerspective::m() const{
    return m_m;
}

inline int QWarpPerspective::interpolation() const{
    return m_interpolation;
}

inline int QWarpPerspective::borderMode() const{
    return m_borderMode;
}

inline QColor QWarpPerspective::borderValue() const{
    return m_borderValue;
}

inline QMat *QWarpPerspective::input(){
    return m_in;
}

inline QMat *QWarpPerspective::output(){
    return m_output;
}

inline void QWarpPerspective::setM(QMat *m){
    m_m = m;
    emit mChanged();
    transform();
}

inline void QWarpPerspective::setInterpolation(int interpolation){
    if (m_interpolation == interpolation)
        return;

    m_interpolation = interpolation;
    emit interpolationChanged();
    transform();
}

inline void QWarpPerspective::setBorderMode(int borderMode){
    if (m_borderMode == borderMode)
        return;

    m_borderMode = borderMode;
    emit borderModeChanged();
    transform();
}

inline void QWarpPerspective::setBorderValue(QColor borderValue){
    if (m_borderValue == borderValue)
        return;

    if ( borderValue.isValid() ){
        m_borderValueInternal = cv::Scalar(
            borderValue.blue(), borderValue.green(), borderValue.red(), borderValue.alpha()
        );
    }

    m_borderValue = borderValue;
    emit borderValueChanged();
    transform();
}

inline void QWarpPerspective::setInput(QMat *mat){
    if ( mat == 0 )
        return;

    m_in = mat;
    emit inputChanged();
    transform();
}

inline void QWarpPerspective::setOutput(QMat *mat){
    if ( mat == 0 )
        return;

    cv::Mat* matData = mat->cvMat();
    if ( implicitWidth() != matData->cols || implicitHeight() != matData->rows ){
        setImplicitWidth(matData->cols);
        setImplicitHeight(matData->rows);
    }
    m_output = mat;
    emit outputChanged();
    transform();
}

#endif // QWARPPERSPECTIVE_H
