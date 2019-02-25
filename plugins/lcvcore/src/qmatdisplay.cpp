/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#include "qmatdisplay.h"
#include "qmatnode.h"
#include "qmatshader.h"
#include <QSGSimpleMaterial>


/**
 *\class QMatDisplay
 *\ingroup plugin-lcvcore
 *\brief Main matrix display class.

 *Extend this class if you want to have a matrix type item that displays on screen. The display parameter is called
 * **output**, which you can access by it's setter QMatDisplay::setOutput() and getter QMatDisplay::output().

 *Within a QML application, if an item is visible, the updatePaintNode() function is called when drawing the element.
 *This can become useful if you have a matrix that requires extra processing only if it will be displayed on screen.
 *when deriving this class, you can override the QMatDisplay::updatePaintNode() function, and implement the actual
 *refurbishing in there. This way, if the element is not visible, you can save some processing time.
 */

/**
 *\brief QMatDisplay constructor
 *
 *Parameters:
 *\a parent
 */
QMatDisplay::QMatDisplay(QQuickItem *parent)
    : QQuickItem(parent)
    , m_output(new QMat())
    , m_linearFilter(true)
{
    setFlag(ItemHasContents, true);
}

/**
 *\brief QMatDisplay constructor

 *Parameters:
 *\a output
 *\a parent
 */
QMatDisplay::QMatDisplay(QMat *output, QQuickItem *parent)
    : QQuickItem(parent)
    , m_output(output)
    , m_linearFilter(true)
{
    setFlag(ItemHasContents, true);
}

/**
 *\brief QMatDisplay destructor
 */
QMatDisplay::~QMatDisplay(){
    delete m_output;
}


/**
 *\property QMatDisplay::output
 *\sa MatDisplay::output
 */

/**
 *\property QMatDisplay::linearFilter
 *\sa MatDisplay::linearFilter
 */

/**
 *\fn void QMatDisplay::setOutput(QMat*)
 *
 *\brief Set the \a mat to be displayed.
*/

/**
 *\fn virtual QSGNode* QMatDisplay::updatePaintNode(QSGNode*, UpdatePaintNodeData*)
 *
 *\brief Updates the scene graph node with the set matrix.

 *Parameters :
 *\a node
 *\a nodeData
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

/**
 *\fn lv::QMatDisplay::linearFilterChanged()
 *\brief Triggered when linearFilter is changed
 */

 /**
 *\fn lv::QMatDisplay::setLinearFilter()
 *\brief Setter for linearFilter
 */

 /**
 *\fn lv::QMatDisplay::outputChanged()
 *\brief Triggered when output is changed
 */
