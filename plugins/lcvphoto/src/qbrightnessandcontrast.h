#ifndef QBRIGHTNESSANDCONTRAST_H
#define QBRIGHTNESSANDCONTRAST_H

#include <QQuickItem>
#include "qmatfilter.h"

class QBrightnessAndContrast : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(double brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
    Q_PROPERTY(double contrast   READ contrast   WRITE setContrast   NOTIFY contrastChanged)

public:
    explicit QBrightnessAndContrast(QQuickItem* parent = 0);
    ~QBrightnessAndContrast();

    double brightness() const;
    double contrast() const;

    virtual void transform(const cv::Mat &in, cv::Mat &out);

    void setBrightness(double brightness);
    void setContrast(double contrast);

signals:
    void brightnessChanged();
    void contrastChanged();

private:
    double m_brightness;
    double m_contrast;
};

inline double QBrightnessAndContrast::brightness() const{
    return m_brightness;
}

inline double QBrightnessAndContrast::contrast() const{
    return m_contrast;
}

inline void QBrightnessAndContrast::setBrightness(double brightness){
    if ( m_brightness == brightness )
        return;

    m_brightness = brightness;
    emit brightnessChanged();
    QMatFilter::transform();
}

inline void QBrightnessAndContrast::setContrast(double contrast){
    if ( m_contrast == contrast )
        return;

    m_contrast = contrast;
    emit contrastChanged();
    QMatFilter::transform();
}

#endif // QBRIGHTNESSANDCONTRAST_H
