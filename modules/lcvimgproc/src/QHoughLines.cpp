#include "QHoughLines.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>

using namespace cv;
using namespace std;

class QHoughLinesPrivate{

public:
    vector<Vec2f> lines;
    bool          linesDirty;
    bool          outDirty;

};

QHoughLines::QHoughLines(QQuickItem *parent)
    : QMatFilter(parent)
    , m_sm(0)
    , m_stn(0)
    , d_ptr(new QHoughLinesPrivate){

    Q_D(QHoughLines);
    d->linesDirty = false;
    d->outDirty   = false;
}

QHoughLines::~QHoughLines(){
    delete d_ptr;
}

void QHoughLines::transform(cv::Mat &in, cv::Mat&){
    Q_D(QHoughLines);
    if ( in.size() != Size(0, 0) )
        HoughLines(in, d->lines, m_rho, m_theta, m_threshold, m_sm, m_stn);
    d->linesDirty = true;
    d->outDirty   = true;
}

QSGNode *QHoughLines::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *nodeData){
    Q_D(QHoughLines);
    if ( d->outDirty ){
        Mat* surface = output()->cvMat();
        surface->create(inputMat()->cvMat()->size(), CV_8UC4);
        surface->setTo(Scalar(0, 0, 0, 0));
        for( size_t i = 0; i < d->lines.size(); ++i ){
            float rho = d->lines[i][0], theta = d->lines[i][1];
            double a  = cos(theta), b = sin(theta);
            double x0 = a * rho, y0 = b * rho;
            Point pt1, pt2;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            line( *surface, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
        }
        d->outDirty = false;
    }
    return QMatDisplay::updatePaintNode(node, nodeData);
}
