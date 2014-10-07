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

#ifndef QMATTRANSFORMATION_HPP
#define QMATTRANSFORMATION_HPP

#include "QMatDisplay.hpp"

/**
 * @brief An abstract filter that transforms an input matrix into it's output.
 *
 * @author Dinu SV
 * @version 1.0.0
 * @ingroup cpp_core
 *
 * Besides the QMatDisplay class it extends, the QMatFilter adds an input element, and a transformation function to further
 * ease implementing filters from Open cv. The transformation function gets called every time the input element is changed.
 *
 * This means that to write a simple filter, all you have to do is extend this class and implement the transformation function.
 * Here's a small example of a filter that transforms a BGR image into grayscale :
 *
 * @code
 * class QMatToGrey : public QMatFilter{
 *
 *     Q_OBJECT
 *
 * public:
 *     QMatToGrey(QQuickItem* parent = 0):QMatFilter(parent){
 *     }
 *
 *     void transform(cv::Mat& in, cv::Mat& out){
 *         cvtColor(in, out, CV_BGR2GREY);
 *     }
 *
 * }
 * @endcode
 *
 * With that all that's left is to register the element to QML, after which you can use it like any other Live CV filter :
 *
 * @code
 * ImRead{
 *      id : src
 *      file : codeDocument.path + 'sample.jpg'
 *      Component.onCompleted : {
 *           var outputdata = output.data()
 *           var rows       = outputdata.rows()
 *           var cols       = outputdata.cols()
 *           var channels   = outputdata.channels()
 *
 *           var vals = outputdata.values()
 *
 *           for ( var i = 0; i < rows; ++i ){
 *               for ( var j = 0; j < cols; ++j ){
 *                   var b = vals[i][j * channels + 0];
 *                   var g = vals[i][j * channels + 1];
 *                   var r = vals[i][j * channels + 2] = 200
 *                   }
 *               }
 *           }
 *      }
 * }
 * @endcode
 */
class Q_LCV_EXPORT QMatFilter : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QMat* input READ inputMat WRITE setInputMat NOTIFY inputChanged)

public:
    explicit QMatFilter(QQuickItem *parent = 0);
    virtual ~QMatFilter();
    
    QMat* inputMat();
    void setInputMat(QMat* mat);

    void transform();
    virtual void transform(cv::Mat& in, cv::Mat& out);

signals:
    void inputChanged();

private:
    QMat* m_in;
    QMat* m_inInternal;
    
};

/**
 * @brief QMatFilter::inputMat
 * @return
 */
inline QMat *QMatFilter::inputMat(){
    return m_in;
}

/**
 * @brief QMatFilter::setInputMat
 * @param mat
 */
inline void QMatFilter::setInputMat(QMat *mat){
    if ( implicitWidth() == 0 || implicitHeight() == 0 ){
        cv::Mat* matData = mat->cvMat();
        setImplicitWidth(matData->cols);
        setImplicitHeight(matData->rows);
    }
    m_in = mat;
    emit inputChanged();
    transform();
}

/**
 * @brief QMatFilter::transform
 */
inline void QMatFilter::transform(){
    transform(*inputMat()->cvMat(), *output()->cvMat());
    emit outputChanged();
    update();
}

#endif // QMATTRANSFORMATION_HPP
