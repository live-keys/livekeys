/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
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

#include "qmatshader.h"

// In some versions of gl.h the following 2 defines are missing

#ifndef GL_UNPACK_ROW_LENGTH
#define GL_UNPACK_ROW_LENGTH              0x0CF2
#endif

#ifndef GL_UNPACK_ALIGNMENT
#define GL_UNPACK_ALIGNMENT               0x0CF5
#endif

/**
 * \class QMatShader
 * \ingroup plugin-lcvcore
 * \brief Matrix shader.
 */

/**
 * \fn const char* QMatShader::vertexShader() const
 * \brief Returns the vertex shader
 */

/**
 * \fn const char* QMatShader::fragmentShader() const
 * \brief Returns the fragment shader.
 */

/**
 * \fn QList<QByteArray> QMatShader::attributes() const
 * \brief Returns the gl program attributes.
 */

/**
 * \fn void QMatShader::updateState(const QMatState *state, const QMatState *oldState)
 * \brief Loads the matrixes texture.
 */


/**
 * \fn void QMatShader::resolveUniforms()
 * \brief Resolves textureId uniform.
 */

/**
 * \brief QMatShader constructor
 */
QMatShader::QMatShader()
    : m_glFunctions()
{
    auto v = QSurfaceFormat::defaultFormat().version();
    if ( v.first > 3 )
        m_shaderVersion = 330;
    else if ( v.first == 3 && v.second >= 3 )
        m_shaderVersion = 330;
    else
        m_shaderVersion = 20;
}

/**
 * \brief Loads a matrix texture into the gpu program. Returns true on success, false otherwise.
 */
bool QMatShader::loadTexture(QMat *mat, int index, bool linearFilter){

    m_glFunctions.glBindTexture(GL_TEXTURE_2D, m_textures[index]);

    // Texture parameters (scaling and edging options);
    GLint resizeFilter = linearFilter ? GL_LINEAR : GL_NEAREST;
    m_glFunctions.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, resizeFilter);
    m_glFunctions.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, resizeFilter);
    m_glFunctions.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    m_glFunctions.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

    // Mat step
    m_glFunctions.glPixelStorei(GL_UNPACK_ALIGNMENT, (mat->internalPtr()->step & 3) ? 1 : 4);

    if ( !mat->internalPtr()->empty() )
        m_glFunctions.glPixelStorei(GL_UNPACK_ROW_LENGTH, (GLint)( mat->internalPtr()->step / mat->internalPtr()->elemSize()) );

    GLint colorFormat = mat->internalPtr()->channels() == 3
            ? GL_RGB  : mat->internalPtr()->channels() == 4
            ? GL_RGBA : GL_LUMINANCE;

    m_glFunctions.glTexImage2D(
         GL_TEXTURE_2D, 0,          // Pyramid level (for mip-mapping) - 0 is the top level
         colorFormat,               // Internal colour format to convert to
         mat->internalPtr()->cols,         // Width
         mat->internalPtr()->rows,         // Height
         0,                         // Border
         colorFormat,               // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)0x80E0
         GL_UNSIGNED_BYTE,          // Image data type
         mat->internalPtr()->ptr()         // The actual image data itself
    );
    m_glFunctions.glPixelStorei(GL_UNPACK_ALIGNMENT,  4);
    m_glFunctions.glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    return true;
}
