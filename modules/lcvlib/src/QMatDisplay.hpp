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

#ifndef QMATDISPLAY_HPP
#define QMATDISPLAY_HPP

#include <QQuickItem>
#include "QLCVGlobal.hpp"
#include "QMat.hpp"

/**
 * @brief Main matrix display class
 *
 * @author Dinu SV
 * @version 1.0.0
 * @ingroup cpp_core
 *
 * Extend this class if you want to have a matrix type item that displays on screen. The display parameter is called <i>output</i>, which
 * you can access by it's setter( setOutput() ) and getter( output() ).
 *
 * Within a QML application, an item might be visible or not, depending if the item's visibility is set to true or false. If the item is
 * visible, the updatePaintNode() function is called when drawing the element. This can become useful if you have a matrix that requires
 * refurbishing only if it's displayed on screen. While extending this class, you can override the updatePaintNode() method, and implement
 * the actual refurbishing in there. This way, if the element is not visible, you can save a few resources when processing the image.
 */
class Q_LCV_EXPORT QMatDisplay : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QMat* output       READ output       WRITE setOutput       NOTIFY outputChanged)
    Q_PROPERTY(bool  linearFilter READ linearFilter WRITE setLinearFilter NOTIFY linearFilterChanged)

public:
    explicit QMatDisplay(QQuickItem *parent = 0);
    QMatDisplay(QMat* output, QQuickItem *parent = 0);
    virtual ~QMatDisplay();

    QMat* output();
    void setOutput(QMat* mat);

    bool linearFilter() const;
    void setLinearFilter(bool filter);

signals:
    void outputChanged();
    void linearFilterChanged();

protected:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *);
    
private:
    QMat* m_output;
    bool  m_linearFilter;

};

/**
 * @brief QMatDisplay::output
 * @return
 */
inline QMat *QMatDisplay::output(){
    return m_output;
}

/**
 * @brief QMatDisplay::setOutput
 * @param mat
 */
inline void QMatDisplay::setOutput(QMat *mat){
    m_output = mat;
    emit outputChanged();
}

/**
 * @brief QMatDisplay::linearFilter
 * @return
 */
inline bool QMatDisplay::linearFilter() const{
    return m_linearFilter;
}

/**
 * @brief QMatDisplay::setLinearFilter
 * @param filter
 */
inline void QMatDisplay::setLinearFilter(bool filter){
    if ( filter != m_linearFilter ){
        m_linearFilter = filter;
        emit linearFilterChanged();
        update();
    }
}

#endif // QMATDISPLAY_HPP
