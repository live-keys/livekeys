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

#include "qmatnode.h"
#include "qmatshader.h"
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
