#include "QStructuringElement.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

QStructuringElement::QStructuringElement(QQuickItem *parent)
    : QQuickItem(parent)
    , m_output(new QMat)
    , m_outputDirty(false)
{
    setFlag(ItemHasContents, false);
}

QMat* QStructuringElement::output() const{
    if ( m_outputDirty )
        *m_output->data() = getStructuringElement(
                m_shape,
                Size(ksize.width(), ksize.height()),
                Point(m_anchor.x(), m_anchor.y()));

    return m_output;
}

QStructuringElement::~QStructuringElement(){
    delete m_output;
}
