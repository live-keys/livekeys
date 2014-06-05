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

#include "QMatShader.hpp"

// In some versions of gl.h the following 2 defines are missing

#ifndef GL_UNPACK_ROW_LENGTH
#define GL_UNPACK_ROW_LENGTH              0x0CF2
#endif

#ifndef GL_UNPACK_ALIGNMENT
#define GL_UNPACK_ALIGNMENT               0x0CF5
#endif

QMatShader::QMatShader(){
}

bool QMatShader::loadTexture(QMat *mat, int index, bool linearFilter){

    glBindTexture(GL_TEXTURE_2D, m_textures[index]);

    // Texture parameters (scaling and edging options);
    GLint resizeFilter = linearFilter ? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, resizeFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, resizeFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

    // Mat step
    glPixelStorei(GL_UNPACK_ALIGNMENT, (mat->data()->step & 3) ? 1 : 4);
	if ( mat->data()->elemSize() != 0 )
        glPixelStorei(GL_UNPACK_ROW_LENGTH, (GLint)( mat->data()->step / mat->data()->elemSize()) );

    GLint colorFormat = mat->data()->channels() == 3
            ? GL_RGB  : mat->data()->channels() == 4
            ? GL_RGBA : GL_LUMINANCE;

    glTexImage2D(
         GL_TEXTURE_2D, 0,          // Pyramid level (for mip-mapping) - 0 is the top level
         colorFormat,               // Internal colour format to convert to
         mat->data()->cols,         // Width
         mat->data()->rows,         // Height
         0,                         // Border
         colorFormat,               // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)0x80E0
         GL_UNSIGNED_BYTE,          // Image data type
         mat->data()->ptr()         // The actual image data itself
    );
    glPixelStorei(GL_UNPACK_ALIGNMENT,  4);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    return true;
}
