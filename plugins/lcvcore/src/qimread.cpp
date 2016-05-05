/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
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

#include "qimread.h"
#include "qmatstate.h"
#include "qmatnode.h"
#include "opencv2/highgui/highgui.hpp"

#include <QSGSimpleMaterial>

/*!
  \qmltype ImRead
  \instantiates QImRead
  \inqmlmodule lcvcore
  \inherits MatDisplay
  \brief Read an image from the hard drive.

  To read the image, all you need is to specify the location :

  \qml
  ImRead{
       file : 'sample.jpg'
  }
  \endqml

  You can load the image from the location where your qml file is saved by using the path from the codeDocument
  property.

  \qml
  ImRead{
       file : codeDocument.path + '/../_images/caltech_buildings_DSCN0246.JPG'
  }
  \endqml
*/

/*!
   \class QImRead
   \inmodule lcvcore_cpp
   \brief Reads an image from the hard drive.
 */

/*!
 * \brief QImRead::QImRead
 * \a parent
 */
QImRead::QImRead(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_iscolor(CV_LOAD_IMAGE_COLOR)
{
}

/*!
 * \brief QImRead::~QImRead
 */
QImRead::~QImRead(){
}

/*!
 * \qmlproperty string ImRead::file
 * \brief The path to the file to load.
 */

/*!
 * \property QImRead::file
 * \sa ImRead::file
 */

void QImRead::componentComplete(){
    QQuickItem::componentComplete();
    loadImage();
}

void QImRead::loadImage(){
    if ( m_file != "" && isComponentComplete() ){
        cv::Mat temp = cv::imread(m_file.toStdString(), m_iscolor);
        if ( !temp.empty() ){
            temp.copyTo(*output()->cvMat());
            setImplicitWidth(output()->cvMat()->size().width);
            setImplicitHeight(output()->cvMat()->size().height);
            emit outputChanged();
            update();
        }
    }
}
