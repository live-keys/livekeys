#ifndef QTONEMAP_H
#define QTONEMAP_H

#include <QQuickItem>
#include "opencv2/photo.hpp"

#include "qmat.h"
#include "qmatdisplay.h"

class QTonemap : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QMat* input        READ input  WRITE setInput NOTIFY inputChanged)
    Q_PROPERTY(QVariantMap params READ params WRITE setParams NOTIFY paramsChanged)

public:
    QTonemap(QQuickItem* parent = nullptr);
    QTonemap(cv::Ptr<cv::Tonemap> tonemapper, QQuickItem* parent = nullptr);
    virtual ~QTonemap();

    QMat* input() const;
    const QVariantMap& params() const;

    void setInput(QMat* input);

protected:
    virtual void initialize(const QVariantMap& options);
    cv::Ptr<cv::Tonemap> tonemapper();
    void initializeTonemapper(cv::Ptr<cv::Tonemap> tonemapper);
    void process();


    virtual void componentComplete();

signals:
    void inputChanged();
    void outputChanged();
    void paramsChanged();

public slots:
    void setParams(const QVariantMap& params);

private:
    QMat*       m_input;
    QVariantMap m_params;
    cv::Mat     m_store;

    cv::Ptr<cv::Tonemap> m_tonemapper;
};

inline QMat *QTonemap::input() const{
    return m_input;
}

inline const QVariantMap &QTonemap::params() const{
    return m_params;
}

inline void QTonemap::setInput(QMat *input){
    m_input = input;
    emit inputChanged();

    process();
}

inline cv::Ptr<cv::Tonemap> QTonemap::tonemapper(){
    return m_tonemapper;
}

inline void QTonemap::setParams(const QVariantMap& params){
    if (m_params == params)
        return;

    m_params = params;

    initialize(params);
    emit paramsChanged();
    process();
}

#endif // QTONEMAP_H
