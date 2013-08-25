#ifndef QCHANNELSELECT_HPP
#define QCHANNELSELECT_HPP

#include "QMatTransformation.hpp"

class QChannelSelect : public QMatTransformation{

    Q_OBJECT
    Q_PROPERTY(int channel READ channelNo WRITE setChannelNo NOTIFY channelChanged )

public:
    explicit QChannelSelect(QQuickItem *parent = 0);
    virtual ~QChannelSelect();

    virtual void transform(cv::Mat &in, cv::Mat &out);

    int channelNo() const;
    void setChannelNo(int channel);

signals:
    void channelChanged();

private:
    int m_channel;
    
};

inline int QChannelSelect::channelNo() const{
    return m_channel;
}

inline void QChannelSelect::setChannelNo(int channel){
    if ( channel != m_channel ){
        m_channel = channel;
        emit channelChanged();
        QMatTransformation::transform();
    }
}

#endif // QCHANNELSELECT_HPP
