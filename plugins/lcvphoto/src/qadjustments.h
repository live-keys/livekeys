#ifndef QADJUSTMENTS_H
#define QADJUSTMENTS_H

#include <QObject>
#include "qmat.h"

namespace lv{
class ViewEngine;
}

class QAdjustments : public QObject{

    Q_OBJECT

public:
    explicit QAdjustments(QObject *parent = nullptr);
    ~QAdjustments();

public slots:
    QMat* levels(QMat* source, const QJSValue& lightness, const QJSValue& channels);
    QJSValue autoLevels(QMat* histogram);
    QMat* brightnessAndContrast(QMat* source, double brightness, double contrast);
    QMat* hueSaturationLightness(QMat* source, int hue, int saturation, int lightness);
    
private:
    lv::ViewEngine* m_engine;

};

#endif // QADJUSTMENTS_H
