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

#include "qmatbuffer.h"

/*!
  \qmltype MatBuffer
  \instantiates QMatBuffer
  \inqmlmodule lcvcore
  \inherits MatDisplay
  \brief Keeps the previous frame.

  The MatBuffer type is usefull when it comes to video playback and you need to keep reference to a previous frame. The
  buffer stores the frame for next usage, so by it's output you actually get the last frame that was passed around in
  the application. A usefull scenario when it comes to use is frame differencing, as in the example under
  samples/imgproc/framedifference.qml

  \quotefile imgproc/framedifference.qml
*/

/*!
   \class QMatBuffer
   \inmodule lcvcore_cpp
   \brief Stores the last frame that was passed around in the application.
 */

/*!
   \brief QMatBuffer constructor
   \a parent
 */
QMatBuffer::QMatBuffer(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_matBuffer(new QMat)
    , m_matCurrent(new QMat){
}

/*!
  \brief QMatBuffer destructor
 */
QMatBuffer::~QMatBuffer(){
}

/*!
  \property QMatBuffer::input
  \sa MatBuffer::input
 */

/*!
  \qmlproperty QMat MatBuffer::input

  Input matrix to store. Whenever a new input is given, the previous one becomes available as output.
 */
