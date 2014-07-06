#include "QHoughLinesP.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>

using namespace cv;
using namespace std;

class QHoughLinesPPrivate{

public :
    vector<Vec4i> lines;
    bool          linesDirty;
    bool          outDirty;

};

QHoughLinesP::QHoughLinesP(QQuickItem *parent)
    : QMatFilter(parent)
    , m_rho(1)
    , m_theta(1)
    , m_threshold(1)
    , m_minLineLength(0)
    , m_maxLineGap(0)
    , m_lineColor(QColor(255, 255, 255))
    , m_lineThickness(1)
    , d_ptr(new QHoughLinesPPrivate){

    Q_D(QHoughLinesP);
    d->linesDirty = false;
    d->outDirty   = false;
}

QHoughLinesP::~QHoughLinesP(){
    delete d_ptr;
}

void QHoughLinesP::transform(cv::Mat &in, cv::Mat&){
    Q_D(QHoughLinesP);
    if ( in.size() != Size(0, 0) ){
        if ( m_rho > 0 && m_theta > 0 && m_threshold > 0 )
            HoughLinesP(in, d->lines, m_rho, m_theta, m_threshold, m_minLineLength, m_maxLineGap );
        else
            qDebug() << "OpenCV Error ( HoughLinesP ) : rho, theta and threshold must be positive";
    }
    d->linesDirty = true;
    d->outDirty   = true;
}

QSGNode *QHoughLinesP::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *nodeData){
    Q_D(QHoughLinesP);
    if ( d->outDirty ){
        Mat* surface = output()->cvMat();
        surface->create(inputMat()->cvMat()->size(), CV_8UC4);
        surface->setTo(Scalar(0, 0, 0, 0));
        for( size_t i = 0; i < d->lines.size(); ++i ){
            line( *surface, Point(d->lines[i][0], d->lines[i][1]),
                Point(d->lines[i][2], d->lines[i][3]), Scalar(m_lineColor.blue(), m_lineColor.green(), m_lineColor.red(), 255), m_lineThickness, 8 );
        }
        d->outDirty = false;
    }
    return QMatDisplay::updatePaintNode(node, nodeData);
}
