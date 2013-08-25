#ifndef QMATSHADER_HPP
#define QMATSHADER_HPP

#include <QSGSimpleMaterialShader>
#include "QMatState.hpp"
#include "QMat.hpp"

class QMatShader : public QSGSimpleMaterialShader<QMatState>{

    QSG_DECLARE_SIMPLE_COMPARABLE_SHADER(QMatShader, QMatState)

public:
    QMatShader();

    const char *vertexShader() const;
    const char *fragmentShader() const;
    QList<QByteArray> attributes() const;
    void updateState(const QMatState *state, const QMatState *oldState);
    void resolveUniforms();

    bool loadTexture(QMat* mat, int index);


private:
    QList<GLuint> m_textures;
    int           m_textureId;

};

inline const char *QMatShader::vertexShader() const{
    return
            "attribute highp vec4 aVertex;                              \n"
            "attribute highp vec2 aTexCoord;                            \n"
            "uniform highp mat4 qt_Matrix;                              \n"
            "varying highp vec2 texCoord;                               \n"
            "void main() {                                              \n"
            "    gl_Position = qt_Matrix * aVertex;                     \n"
            "    texCoord[0] = aTexCoord.x;"
            "    texCoord[1] = aTexCoord.y;"
            "}";
}

inline const char *QMatShader::fragmentShader() const{
    return
            "uniform lowp float qt_Opacity;                             \n"
            "varying highp vec2 texCoord;                               \n"
            "uniform sampler2D textures[1];                             \n"
            "void main ()                                               \n"
            "{                                                          \n"
            "   highp vec4 textureColor = texture2D( textures[0], texCoord.st );"
            "   gl_FragColor = vec4(textureColor.b, textureColor.g, textureColor.r, qt_Opacity);"
            "}";
}

inline QList<QByteArray> QMatShader::attributes() const{
    return QList<QByteArray>() << "aVertex" << "aTexCoord";
}

inline void QMatShader::updateState(const QMatState *state, const QMatState *){

    if ( state->mat != 0 ){
        glEnable(GL_TEXTURE_2D);
        if ( !state->textureSync ){
            if ( state->textureIndex == -1 ){
                state->textureIndex = m_textures.length();
                m_textures.append(0);
            }

            loadTexture(state->mat, state->textureIndex);
            state->textureSync = true;
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textures[state->textureIndex]);
        program()->setUniformValue(m_textureId, 0);
    }
}

inline void QMatShader::resolveUniforms(){
    m_textureId = program()->uniformLocation("textures[0]");
}

#endif // QMATSHADER_HPP
