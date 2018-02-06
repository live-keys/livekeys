/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/
#include "qchannelselect.h"
#include "opencv2/imgproc.hpp"

/*!
  \qmltype ChannelSelect
  \instantiates QChannelSelect
  \inqmlmodule lcvimgproc
  \inherits MatFilter
  \brief Selects an image channel by its index.

  \quotefile imgproc/framedifference.qml
*/

/*!
  \class QChannelSelect
  \internal
  \inmodule lcvimgproc_cpp
  \brief Selects an image channel by its index.
 */

QChannelSelect::QChannelSelect(QQuickItem *parent)
    : QMatFilter(parent)
    , m_channel(0)
    , m_channels(3)
{
}

QChannelSelect::~QChannelSelect(){
}

/*!
  \qmlproperty int ChannelSelect::channel

  Channel number.
 */

/*!
  \brief Filter function

  Parameters:
  \a in
  \a out
 */
void QChannelSelect::transform(const cv::Mat &in, cv::Mat &out){
    if ( out.channels() == 3 )
        cv::cvtColor(out, out, CV_BGR2GRAY);
    if ( !in.empty() ){
        if ( in.channels() == 1 ){
            in.copyTo(out);
            return;
        } else if ( in.channels() == 3 ){
            cv::split(in, m_channels);
            m_channels[m_channel].copyTo(out);
        }
    }
}
