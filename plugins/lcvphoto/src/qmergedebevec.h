#ifndef QMERGEDEBEVEC_H
#define QMERGEDEBEVEC_H

#include <QObject>
#include <QQmlParserStatus>
#include "qmatlist.h"
#include "opencv2/photo.hpp"

class QMergeDebevec : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QMatList* input    READ input    WRITE setInput    NOTIFY inputChanged)
    Q_PROPERTY(QList<qreal> times   READ times    WRITE setTimes    NOTIFY timesChanged)
    Q_PROPERTY(QMat* response     READ response WRITE setResponse NOTIFY responseChanged)
    Q_PROPERTY(QMat* output       READ output   NOTIFY outputChanged)

public:
    explicit QMergeDebevec(QObject *parent = nullptr);
    ~QMergeDebevec();

    QMatList* input() const;
    const QList<qreal>& times() const;
    QMat* output() const;
    QMat* response() const;

    void setInput(QMatList* input);
    void setTimes(QList<qreal> times);
    void setResponse(QMat* response);

    void classBegin() Q_DECL_OVERRIDE{}
    void componentComplete() Q_DECL_OVERRIDE;

signals:
    void inputChanged();
    void timesChanged();
    void outputChanged();
    void responseChanged();

private:
    void filter();
    bool isComponentComplete();

    QMatList*   m_input;
    QList<qreal>  m_times;
    QMat*       m_response;
    QMat*       m_output;

    cv::Ptr<cv::MergeDebevec> m_mergeDebevec;

    bool        m_componentComplete;
};

inline QMatList *QMergeDebevec::input() const{
    return m_input;
}

inline const QList<qreal> &QMergeDebevec::times() const{
    return m_times;
}

inline QMat *QMergeDebevec::output() const{
    return m_output;
}

inline void QMergeDebevec::setInput(QMatList *input){
    m_input = input;
    emit inputChanged();

    filter();
}

inline void QMergeDebevec::setTimes(QList<qreal> times){
    if (m_times == times)
        return;

    m_times = times;
    emit timesChanged();

    filter();
}

inline bool QMergeDebevec::isComponentComplete(){
    return m_componentComplete;
}

inline void QMergeDebevec::componentComplete(){
    m_componentComplete = true;
    filter();
}

inline void QMergeDebevec::setResponse(QMat *response){
    m_response = response;
    emit responseChanged();

    filter();
}

inline QMat *QMergeDebevec::response() const{
    return m_response;
}

#endif // QMERGEDEBEVEC_H
