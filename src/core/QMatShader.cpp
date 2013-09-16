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

bool QMatShader::loadTexture(QMat *mat, int index){

    glBindTexture(GL_TEXTURE_2D, m_textures[index]);

    // Give texture parameters (scaling and edging options)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

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

    return true;
}
