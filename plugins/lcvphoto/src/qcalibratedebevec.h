#ifndef QCALIBRATEDEBEVEC_H
#define QCALIBRATEDEBEVEC_H

#include <QObject>
#include <QQmlParserStatus>
#include "qmatlist.h"
#include "opencv2/photo.hpp"

class QCalibrateDebevec : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QMatList* input    READ input  WRITE setInput  NOTIFY inputChanged)
    Q_PROPERTY(QList<qreal> times READ times  WRITE setTimes  NOTIFY timesChanged)
    Q_PROPERTY(QVariantMap params READ params WRITE setParams NOTIFY paramsChanged)
    Q_PROPERTY(QMat* output       READ output NOTIFY outputChanged)

public:
    explicit QCalibrateDebevec(QObject *parent = nullptr);
    ~QCalibrateDebevec();

    QMatList* input() const;
    const QList<qreal>& times() const;
    const QVariantMap& params() const;
    QMat* output() const;

    void setInput(QMatList* input);
    void setTimes(QList<qreal> times);

    void classBegin() Q_DECL_OVERRIDE{}
    void componentComplete() Q_DECL_OVERRIDE;

signals:
    void inputChanged();
    void timesChanged();
    void paramsChanged();
    void outputChanged();

public slots:
    void setParams(const QVariantMap& params);

private:
    void filter();
    bool isComponentComplete();

    QMatList*    m_input;
    QList<qreal> m_times;
    QVariantMap  m_params;
    QMat*        m_output;

    cv::Ptr<cv::CalibrateDebevec> m_calibrateDebevec;

    bool        m_componentComplete;
};

inline QMatList *QCalibrateDebevec::input() const{
    return m_input;
}

inline const QList<qreal> &QCalibrateDebevec::times() const{
    return m_times;
}

inline const QVariantMap &QCalibrateDebevec::params() const{
    return m_params;
}

inline QMat *QCalibrateDebevec::output() const{
    return m_output;
}

inline void QCalibrateDebevec::setInput(QMatList *input){
    m_input = input;
    emit inputChanged();

    filter();
}

inline void QCalibrateDebevec::setTimes(QList<qreal> times){
    if (m_times == times)
        return;

    m_times = times;
    emit timesChanged();

    filter();
}

inline bool QCalibrateDebevec::isComponentComplete(){
    return m_componentComplete;
}

inline void QCalibrateDebevec::componentComplete(){
    m_componentComplete = true;
    filter();
}

#endif // QCALIBRATEDEBEVEC_H
