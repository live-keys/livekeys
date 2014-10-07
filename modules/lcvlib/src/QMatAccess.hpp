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
#ifndef QMATACCESS_HPP
#define QMATACCESS_HPP

#include <QQuickItem>
#include "QMat.hpp"
#include "QLCVGlobal.hpp"

#include <QAbstractListModel>

/**
 * @page qml_mataccess Accessing a matrixes pixels
 * @ingroup qml_core
 *
 * You can access a matrix's pixels from QML by using the data() function on a matrix element. The data() function returns a matrix data
 * accessor type, which can then be used to access the properties of the selected matrix. Here's a how you can access pixel values from a
 * RGB matrix.
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
 *
 * The rows(), cols() and channels() functions are self explanatory, while for the values() function it's important to note that it returns a 2D
 * array of rows and columns, and that each of channels are merged into the same row.
 *
 * Changing a matrixes values can be done by changing the array returned by the values() function, and then setting it back by using the setValues() function.
 *
 */


/**
 * @brief The QMatAccess class
 * @author Dinu SV
 * @version 1.0.0
 * @ingroup cpp_core
 *
 * QML documentation for this class is available (here)[@ref qml_mataccess].
 */
class Q_LCV_EXPORT QMatAccess : public QQuickItem{

    Q_OBJECT

public:
    explicit QMatAccess(QQuickItem* parent = 0);
    QMatAccess(QMat* mat, QQuickItem *parent = 0);
    ~QMatAccess();

public slots:
    int rows() const;
    int cols() const;
    int channels() const;

    int depth() const;

    QVariantList values();
    void setValues(QVariantList values);

private:
    void setUpCache();


    QMat* m_mat;

    uchar* m_dataPtr;
    int    m_typeStep;
    int    m_channels;

    QVariantList  m_values;

};

/**
 * @brief QMatAccess::cols
 * @return
 */
inline int QMatAccess::cols() const{
    return m_mat->cvMat()->cols;
}

/**
 * @brief QMatAccess::rows
 * @return
 */
inline int QMatAccess::rows() const{
    return m_mat->cvMat()->rows;
}

/**
 * @brief QMatAccess::channels
 * @return
 */
inline int QMatAccess::channels() const{
    return m_channels;
}

/**
 * @brief QMatAccess::depth
 * @return
 */
inline int QMatAccess::depth() const{
    return m_mat->cvMat()->depth();
}

#endif // QMATACCESS_HPP
