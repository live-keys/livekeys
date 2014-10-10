#include "QCalcOpticalFlowPyrLK.hpp"
#include "opencv2/video/tracking.hpp"
#include "QStateContainer.hpp"

using namespace cv;

class QCalcOpticalFlowPointState{

public:
    std::vector<Point2f> currentPoints;
    std::vector<Point2f> prevPoints;
    std::vector<uchar>   status;
    std::vector<float>   err;

};

class QCalcOpticalFlowPyrLKPrivate{

public:
    QCalcOpticalFlowPyrLKPrivate();
    ~QCalcOpticalFlowPyrLKPrivate();

    void calculateFlow(cv::Mat& input);
    void draw(cv::Mat& frame, cv::Mat& m);
    void addPoint(const cv::Point& p);

    QCalcOpticalFlowPointState* pointState();

    const QString& stateId() const;
    void setStateId(const QString& id);

    size_t totalPoints() const;

public:
    cv::Mat              gray;
    cv::Mat              prevGray;
    cv::Size             winSize;
    cv::Scalar           pointColor;
    cv::TermCriteria     termcrit;
    int                  maxLevel;
    int                  flags;
    double               minEigThreshold;

private:
    QString                     m_stateId;
    QCalcOpticalFlowPointState* m_pointState;
};

// QCalcOpticalFlowPyrLKPrivate Implementation
// -------------------------------------------

QCalcOpticalFlowPyrLKPrivate::QCalcOpticalFlowPyrLKPrivate()
    : winSize(21, 21)
    , pointColor(0, 255, 0)
    , termcrit(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01)
    , maxLevel(3)
    , flags(0)
    , minEigThreshold(1e-4)
    , m_pointState(0){

}

QCalcOpticalFlowPyrLKPrivate::~QCalcOpticalFlowPyrLKPrivate(){
    if ( m_stateId == "" ) // otherwise leave it to the QStateContiner
        delete m_pointState;
}

void QCalcOpticalFlowPyrLKPrivate::calculateFlow(cv::Mat& input){
    if ( input.channels() == 1 )
        gray = input;
    else
        cvtColor(input, gray, CV_BGR2GRAY);

    QCalcOpticalFlowPointState* ps = pointState();

    if ( !ps->currentPoints.empty() ){

        std::swap(ps->currentPoints, ps->prevPoints);
        if ( prevGray.empty() )
            gray.copyTo(prevGray);
        else {

            calcOpticalFlowPyrLK(
                        prevGray, gray, ps->prevPoints, ps->currentPoints,
                        ps->status, ps->err, winSize, maxLevel, termcrit, flags, minEigThreshold);

            gray.copyTo(prevGray);

            size_t k = 0;
            for ( size_t i = 0; i < ps->currentPoints.size(); ++i ){
                if ( !ps->status[i] )
                    continue;
                ps->currentPoints[k++] = ps->currentPoints[i];
            }
            ps->currentPoints.resize(k);
        }
    }
}

void QCalcOpticalFlowPyrLKPrivate::draw(cv::Mat& frame, cv::Mat& m){
    frame.copyTo(m);
    for ( size_t i = 0; i < pointState()->currentPoints.size(); ++i )
        circle(m, pointState()->currentPoints[i], 3, pointColor, -1, 8);
}

void QCalcOpticalFlowPyrLKPrivate::addPoint(const cv::Point& p){
    pointState()->currentPoints.push_back(p);
}

QCalcOpticalFlowPointState* QCalcOpticalFlowPyrLKPrivate::pointState(){

    if ( !m_pointState ){
        if ( m_stateId != "" ){
            QStateContainer<QCalcOpticalFlowPointState>& stateCont =
                    QStateContainer<QCalcOpticalFlowPointState>::instance();

            m_pointState = stateCont.state(m_stateId);
            if ( m_pointState == 0 ){
                m_pointState = new QCalcOpticalFlowPointState;
                stateCont.registerState(m_stateId, m_pointState);
            }
        } else {
            qDebug() << "QCalcOpticalFlowPyrLK does not have a stateId assigned and cannot save " <<
                        "it\'s state over multiple code compilations. Set a unique stateId to " <<
                        "avoid this problem. ";
            m_pointState = new QCalcOpticalFlowPointState;
        }
    }
    return m_pointState;
}

const QString&QCalcOpticalFlowPyrLKPrivate::stateId() const{
    return m_stateId;
}

void QCalcOpticalFlowPyrLKPrivate::setStateId(const QString& id){
    if ( m_stateId == "" )
        if ( m_pointState != 0 )
            delete m_pointState;

    m_pointState = 0;
    m_stateId    = id;
}

size_t QCalcOpticalFlowPyrLKPrivate::totalPoints() const{
    if ( m_pointState )
        return m_pointState->currentPoints.size();
    return 0;
}


// QCalcOpticalFlowPyrLK Implementation
// ------------------------------------

/**
 * @brief QCalcOpticalFlowPyrLK Constructor
 * @param parent
 */
QCalcOpticalFlowPyrLK::QCalcOpticalFlowPyrLK(QQuickItem *parent)
    : QMatFilter(parent)
    , d_ptr(new QCalcOpticalFlowPyrLKPrivate){

}

QCalcOpticalFlowPyrLK::~QCalcOpticalFlowPyrLK(){
    delete d_ptr;
}

void QCalcOpticalFlowPyrLK::addPoint(const QPoint& point){
    Q_D(QCalcOpticalFlowPyrLK);
    d->addPoint(Point(point.x(), point.y()));
}

QList<QPoint> QCalcOpticalFlowPyrLK::points(){
    Q_D(QCalcOpticalFlowPyrLK);

    QCalcOpticalFlowPointState* ps = d->pointState();
    QList<QPoint> base;

    for ( size_t i = 0; i < ps->currentPoints.size(); ++i ){
        base.append(QPoint(ps->currentPoints[i].x, ps->currentPoints[i].y));
    }
    return base;
}

int QCalcOpticalFlowPyrLK::totalPoints() const{
    Q_D(const QCalcOpticalFlowPyrLK);
    return (int)d->totalPoints();
}

QSize QCalcOpticalFlowPyrLK::winSize() const{
    Q_D(const QCalcOpticalFlowPyrLK);
    return QSize(d->winSize.width, d->winSize.height);
}

void QCalcOpticalFlowPyrLK::setWinSize(const QSize& winSize){
    Q_D(QCalcOpticalFlowPyrLK);
    if ( d->winSize.width != winSize.width() || d->winSize.height != winSize.height() ){
        d->winSize = Size(winSize.width(), winSize.height());
        emit winSizeChanged();
    }
}

int QCalcOpticalFlowPyrLK::maxLevel() const{
    Q_D(const QCalcOpticalFlowPyrLK);
    return d->maxLevel;
}

void QCalcOpticalFlowPyrLK::setMaxLevel(int maxLevel){
    Q_D(QCalcOpticalFlowPyrLK);
    if (d->maxLevel != maxLevel){
        d->maxLevel = maxLevel;
        emit maxLevelChanged();
    }
}

int QCalcOpticalFlowPyrLK::flags() const{
    Q_D(const QCalcOpticalFlowPyrLK);
    return d->flags;
}

void QCalcOpticalFlowPyrLK::setFlags(int flags){
    Q_D(QCalcOpticalFlowPyrLK);
    if (d->flags != flags){
        d->flags = flags;
        emit flagsChanged();
    }
}

double QCalcOpticalFlowPyrLK::minEigThreshold() const{
    Q_D(const QCalcOpticalFlowPyrLK);
    return d->minEigThreshold;
}

void QCalcOpticalFlowPyrLK::setMinEigThreshold(double minEigThreshold){
    Q_D(QCalcOpticalFlowPyrLK);
    if ( d->minEigThreshold != minEigThreshold ){
        d->minEigThreshold = minEigThreshold;
        emit minEigThresholdChanged();
    }
}

const QString&QCalcOpticalFlowPyrLK::stateId() const{
    Q_D(const QCalcOpticalFlowPyrLK);
    return d->stateId();
}

void QCalcOpticalFlowPyrLK::setStateId(const QString& id){
    Q_D(QCalcOpticalFlowPyrLK);
    if ( d->stateId() != id ){
        d->setStateId(id);
        emit stateIdChanged();
    }
}

void QCalcOpticalFlowPyrLK::transform(Mat& in, Mat&){
    Q_D(QCalcOpticalFlowPyrLK);
    d->calculateFlow(in);
}

QSGNode*QCalcOpticalFlowPyrLK::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData*nodeData){
    Q_D(QCalcOpticalFlowPyrLK);
    d->draw( *(inputMat()->cvMat()), *(output()->cvMat()) );

    return QMatDisplay::updatePaintNode(node, nodeData);
}
