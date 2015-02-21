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

#include "QMatDisplay.hpp"
#include "QMatNode.hpp"
#include "QMatShader.hpp"
#include <QSGSimpleMaterial>


/*!
  \qmltype MatDisplay
  \instantiates QMatDisplay
  \inqmlmodule lcvcore
  \inherits Item
  \brief Simple matrix display element.

  This type serves as a base for all other live cv types that require displaying a matrix, which is available in its
  output property. You can choose wether smoothing occurs when scaling the image for display by enabling linear
  filtering.
 */

/*!
  \class QMatDisplay
  \inmodule lcvlib_cpp
  \brief Main matrix display class.

  Extend this class if you want to have a matrix type item that displays on screen. The display parameter is called
  \b{output}, which you can access by it's setter QMatDisplay::setOutput() and getter QMatDisplay::output().

  Within a QML application, if an item is visible, the updatePaintNode() function is called when drawing the element.
  This can become useful if you have a matrix that requires extra processing only if it will be displayed on screen.
  when deriving this class, you can override the QMatDisplay::updatePaintNode() function, and implement the actual
  refurbishing in there. This way, if the element is not visible, you can save some processing time.
 */

/*!
  \brief QMatDisplay constructor

  Parameters:
  \a parent
 */
QMatDisplay::QMatDisplay(QQuickItem *parent)
    : QQuickItem(parent)
    , m_output(new QMat())
    , m_linearFilter(true)
{
    setFlag(ItemHasContents, true);

}

/*!
  \brief QMatDisplay constructor

  Parameters:
  \a output
  \a parent
 */
QMatDisplay::QMatDisplay(QMat *output, QQuickItem *parent)
    : QQuickItem(parent)
    , m_output(output)
    , m_linearFilter(true)
{
    setFlag(ItemHasContents, true);
}

/*!
  \brief QMatDisplay destructor
 */
QMatDisplay::~QMatDisplay(){
    delete m_output;
}


/*!
  \property QMatDisplay::output
  \sa MatDisplay::output
 */

/*!
  \qmlproperty Mat MatDisplay::output

  This property holds the output element to display on screen.
 */


/*!
  \property QMatDisplay::linearFilter
  \sa MatDisplay::linearFilter
 */

/*!
  \qmlproperty bool MatDisplay::linearFilter

  If set to true, linear filtering will occur when scaling the image on the screen. Default value is true.
 */

/*!
  \fn virtual QSGNode* QMatDisplay::updatePaintNode(QSGNode*, UpdatePaintNodeData*)

  \brief Updates the scene graph node with the set matrix.

  Parameters :
  \a node
  \a nodeData
 */


QSGNode *QMatDisplay::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *){
    QMatNode *n = static_cast<QMatNode*>(node);
    if (!node)
        n = new QMatNode();

    QSGGeometry::updateTexturedRectGeometry(n->geometry(), boundingRect(), QRectF(0, 0, 1, 1));
    QMatState* state = static_cast<QSGSimpleMaterial<QMatState>*>(n->material())->state();
    state->mat          = m_output;
    state->textureSync  = false;
    state->linearFilter = m_linearFilter;
    n->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);

    return n;
}
