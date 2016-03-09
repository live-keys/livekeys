/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/
#ifndef QCHANNELSELECT_H
#define QCHANNELSELECT_H

#include "qmatfilter.h"

class QChannelSelect : public QMatFilter{

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

    std::vector<cv::Mat> m_channels;
    
};

inline int QChannelSelect::channelNo() const{
    return m_channel;
}

inline void QChannelSelect::setChannelNo(int channel){
    if ( channel != m_channel ){
        m_channel = channel;
        emit channelChanged();
        QMatFilter::transform();
    }
}

#endif // QCHANNELSELECT_H
