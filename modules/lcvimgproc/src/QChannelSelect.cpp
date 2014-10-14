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
#include "QChannelSelect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

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
  \inmodule lcvimgproc_cpp
  \brief Selects an image channel by its index.
 */

/*!
  \brief QChannelSelect constructor

  Parameters:
  \a parent
 */
QChannelSelect::QChannelSelect(QQuickItem *parent)
    : QMatFilter(parent)
    , m_channel(0)
    , m_channels(3)
{
}

/*!
  \brief QChannelSelect destructor
 */
QChannelSelect::~QChannelSelect(){
}

/*!
  \brief Filter function

  Parameters:
  \a in
  \a out
 */
void QChannelSelect::transform(cv::Mat &in, cv::Mat &out){
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
