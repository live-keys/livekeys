#ifndef QSTITCHER_H
#define QSTITCHER_H

#include <QQuickItem>
#include "opencv2/stitching.hpp"
#include "qmatdisplay.h"
#include "qmatlist.h"

class QStitcher : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QMatList* input    READ input  WRITE setInput  NOTIFY inputChanged)
    Q_PROPERTY(QVariantMap params READ params WRITE setParams NOTIFY paramsChanged)

public:
    enum Mode{
        Panorma = cv::Stitcher::PANORAMA,
        Scans = cv::Stitcher::SCANS
    };
    Q_ENUM(Mode)

    enum Status{
        Ok = cv::Stitcher::OK,
        ErrNeedMoreFiles = cv::Stitcher::ERR_NEED_MORE_IMGS,
        ErrHomographyEstFail = cv::Stitcher::ERR_HOMOGRAPHY_EST_FAIL,
        ErrCameraParamsAdjustFail = cv::Stitcher::ERR_CAMERA_PARAMS_ADJUST_FAIL
    };
    Q_ENUM(Status)

public:
    QStitcher(QQuickItem* parent = nullptr);

    QMatList* input() const;
    void setInput(QMatList* input);

    const QVariantMap &params() const;

signals:
    void inputChanged();
    void error(int status);

    void paramsChanged(QVariantMap params);

public slots:
    void setParams(const QVariantMap& params);

private:
    void filter();

    QMatList*             m_input;
    cv::Ptr<cv::Stitcher> m_stitcher;
    QVariantMap m_params;
};

inline QMatList *QStitcher::input() const{
    return m_input;
}

inline const QVariantMap& QStitcher::params() const{
    return m_params;
}

inline void QStitcher::setInput(QMatList *input){
    if (m_input == input)
        return;

    m_input = input;
    emit inputChanged();

    filter();
}

#endif // QSTITCHER_H
