#include "QMatValues.hpp"
#include <QSGTexture>

QMatValues::QMatValues(QQuickItem *parent) :
    QQuickItem(parent),
    m_texture(0)
{
    // TCODE
    setFlag(ItemHasContents);
    m_surface = QImage("D:/test.jpg");
}

QMatValues::~QMatValues(){
}

QSGNode *QMatValues::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *){
    return 0;
    /*QSGSimpleTextureNode *n = static_cast<QSGSimpleTextureNode*>(node);
    if (!node)
        n = new QSGSimpleTextureNode();

    if ( m_texture->() == QSGTexture::None )

} else {
if (m_texture->status() == QSGTexture::Null) {
GLuint textureId;
glGenTextures(1, &textureId);
glBindTexture(GL_TEXTURE_2D, textureId);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
m_texture->setTextureId(textureId);
}
const QImage image = m_surface->image();
glBindTexture(GL_TEXTURE_2D, m_texture->textureId());
glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, image.width(), image.height(), 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, image.bits());
m_texture->setAlphaChannel(image.hasAlphaChannel());
}
m_texture->setTextureSize(m_surface->geometry().size());
m_texture->setOwnsTexture(true);
m_texture->setHasMipmaps(false);
m_texture->setStatus(QSGTexture::Ready);

n

    QSGGeometry::updateTexturedRectGeometry(n->geometry(), boundingRect(), QRectF(0, 0, 1, 1));
    static_cast<QSGSimpleMaterial<QMatState>*>(n->material())->state()->mat         = m_output;
    static_cast<QSGSimpleMaterial<QMatState>*>(n->material())->state()->textureSync = false;
    n->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
    return n;*/
}

/*

#include "fboinsgrenderer.h"
#include "logorenderer.h"

#include <QtGui/QOpenGLFramebufferObject>

#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>
#include <QOpenGLPaintDevice>

#include <QPainter>


class TextureNode : public QObject, public QSGSimpleTextureNode
{
    Q_OBJECT

public:
    TextureNode(QQuickWindow *window)
        : m_fbo(0)
        , m_texture(0)
        , m_window(window)
    {
        //connect(m_window, SIGNAL(beforeRendering()), this, SLOT(renderFBO()));
    }

    ~TextureNode()
    {
        delete m_texture;
        delete m_fbo;
    }

public slots:
    void renderFBO()
    {
        QSize size = rect().size().toSize();

        if (!m_fbo) {

            QOpenGLFramebufferObjectFormat format;
            format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
            m_fbo = new QOpenGLFramebufferObject(size, format);
            m_texture = m_window->createTextureFromId(m_fbo->texture(), size);
            setTexture(m_texture);
        }

        m_fbo->bind();

        QOpenGLPaintDevice device(size);
        device.setPaintFlipped(true);
        QPainter painter;
        QRect drawRect(0, 0, size.width(), size.height());
        painter.begin(&device);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing );

        painter.fillRect(drawRect, Qt::blue);

        //painter.drawTiledPixmap(drawRect, QPixmap(":/qt-project.org/qmessagebox/images/qtlogo-64.png"));

        painter.setPen(QPen(Qt::green, 5));
        painter.setBrush(Qt::red);
        painter.drawEllipse(0, 0, 100, 100);
        painter.drawEllipse(100, 0, 200, 400);

        painter.setPen(QPen(Qt::white, 0));
        QFont font;
        font.setPointSize(12);
        font.setFamily("Courier New");
        painter.setFont(font);
        painter.drawText(drawRect, "Hello FBO");

        painter.end();
        //m_fbo->release();
    }

private:
    QOpenGLFramebufferObject *m_fbo;
    QSGTexture *m_texture;
    QQuickWindow *m_window;
};



FboInSGRenderer::FboInSGRenderer()
{
    setFlag(ItemHasContents, true);
}


QSGNode *FboInSGRenderer::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    // Don't bother with resize and such, just recreate the node from scratch
    // when geometry changes.
    TextureNode* node = static_cast<TextureNode*>(oldNode);
    if (!oldNode)
        node = new TextureNode(window());

    node->setRect(boundingRect());
    node->renderFBO();

    return node;
}

#include "fboinsgrenderer.moc"
*/
