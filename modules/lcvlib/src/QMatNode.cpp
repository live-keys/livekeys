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

#include "QMatNode.hpp"
#include "QMatShader.hpp"
#include <QSGSimpleMaterial>

/*!
  \class QMatNode
  \inmodule lcvlib_cpp
  \brief Node containing matrix state.
 */

/*!
 * \brief QMatNode constructor
 */
QMatNode::QMatNode()
    : m_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4)
{
      setGeometry(&m_geometry);

      QSGSimpleMaterial<QMatState> *material = QMatShader::createMaterial();
      material->setFlag(QSGMaterial::Blending);
      setMaterial(material);
      setFlag(OwnsMaterial);

}
