#include "QSimpleBlobDetector.hpp"
#include "opencv2/features2d/features2d.hpp"

QSimpleBlobDetector::QSimpleBlobDetector(QQuickItem* parent)
    : QFeatureDetector(new cv::SimpleBlobDetector, parent)
{
}

QSimpleBlobDetector::~QSimpleBlobDetector(){
}

void QSimpleBlobDetector::initialize(const QVariantMap &settings){
    cv::SimpleBlobDetector::Params params;

    if ( settings.contains("thresholdStep") )
        params.thresholdStep       = settings["thresholdStep"].toFloat();
    if ( settings.contains("minThreshold") )
        params.minThreshold        = settings["minThreshold"].toFloat();
    if ( settings.contains("maxThreshold") )
        params.maxThreshold        = settings["maxThreshold"].toFloat();
    if ( settings.contains("minRepeatability") )
        params.minRepeatability    = static_cast<size_t>(settings["minRepeatability"].toLongLong());
    if ( settings.contains("minDistBetweenBlobs") )
        params.minDistBetweenBlobs = settings["minDistBetweenBlobs"].toFloat();

    if ( settings.contains("blobColor") ){
        params.filterByColor = true;
        params.blobColor     = static_cast<uchar>(settings["blobColor"].toUInt());
    }

    if ( settings.contains("minArea") ){
        params.filterByArea = true;
        params.minArea      = settings["minArea"].toFloat();
    }
    if ( settings.contains("maxArea") ){
        params.filterByArea = true;
        params.maxArea      = settings["maxArea"].toFloat();
    }

    if ( settings.contains("minCircularity") ){
        params.filterByCircularity = true;
        params.minCircularity      = settings["minCircularity"].toFloat();
    }
    if ( settings.contains("maxCircularity") ){
        params.filterByCircularity = true;
        params.maxCircularity      = settings["maxCircularity"].toFloat();
    }

    if ( settings.contains("minInertiaRatio") ){
        params.filterByInertia = true;
        params.minInertiaRatio = settings["minInertiaRatio"].toFloat();
    }
    if ( settings.contains("maxInertiaRatio") ){
        params.filterByInertia = true;
        params.maxInertiaRatio = settings["maxInertiaRatio"].toFloat();
    }

    if ( settings.contains("minConvexity") ){
        params.filterByConvexity = true;
        params.minConvexity      = settings["minConvexity"].toFloat();
    }
    if ( settings.contains("maxConvexity") ){
        params.filterByConvexity = true;
        params.maxConvexity      = settings["maxConvexity"].toFloat();
    }

    initializeDetector(new cv::SimpleBlobDetector(params));
}
