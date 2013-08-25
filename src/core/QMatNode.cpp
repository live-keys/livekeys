#include "QMatNode.hpp"
#include "QMatShader.hpp"
#include <QSGSimpleMaterial>

QMatNode::QMatNode():
    m_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4){

      setGeometry(&m_geometry);

      QSGSimpleMaterial<QMatState> *material = QMatShader::createMaterial();
      material->setFlag(QSGMaterial::Blending);
      setMaterial(material);
      setFlag(OwnsMaterial);

}
