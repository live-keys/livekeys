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

#include "QImRead.hpp"
#include "QMatState.hpp"
#include "QMatNode.hpp"
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
    loadImage();
}

void QImRead::loadImage(){
    if ( m_file != "" && isComponentComplete() ){
        cv::Mat temp = cv::imread(file.toStdString(), m_iscolor);
        if ( !temp.empty() ){
            temp.copyTo(*output()->cvMat());
            setImplicitWidth(output()->cvMat()->size().width);
            setImplicitHeight(output()->cvMat()->size().height);
            emit outputChanged();
            update();
        }
    }
}
