#include "qgeometry.h"

#include "live/qmlerror.h"
#include "live/visuallogqt.h"
#include "live/exception.h"
#include "live/viewengine.h"
#include "qtransformations.h"
#include "cvextras.h"
#include "qqmlapplicationengine.h"
#include "live/viewcontext.h"

QGeometry::QGeometry(QObject *parent)
    : QObject(parent)
{
}

QMat *QGeometry::resize(QMat *input, QSize size, int interpolation){
    if ( !input || size.width() == 0)
        return nullptr;
    QMat* m = new QMat;
    cv::resize(input->internal(), m->internal(), cv::Size(size.width(), size.height()), 0, 0, interpolation);
    return m;
}

QMat *QGeometry::scale(QMat *input, double fx, double fy, int interpolation){
    if ( !input )
        return nullptr;
    if ( fx <= 0 || fy <= 0 )
        return nullptr;

    QMat* m = new QMat;
    try {
        cv::resize(input->internal(), m->internal(), cv::Size(), fx, fy, interpolation);
    } catch (cv::Exception& e) {
        qWarning("%s", e.what());
    }

    return m;
}

QMat *QGeometry::resizeBy(QMat *input, QJSValue ob, int interpolation){
    if ( !input )
        return nullptr;

    if ( ob.hasOwnProperty("w") ){

        double s = ob.property("w").toInt() / input->internal().cols;
        return scale(input, s, s, interpolation);

    } else if ( ob.hasOwnProperty("h") ){

        double s = ob.property("h").toInt() / input->internal().cols;
        return scale(input, s, s, interpolation);
    }

    return nullptr;
}

QMat *QGeometry::rotate(QMat *m, double degrees){
    if ( !m )
        return nullptr;

    QMat* r = new QMat;
    cv::Point2f ptCp(
        static_cast<float>(m->internal().cols * 0.5),
        static_cast<float>(m->internal().rows * 0.5));

    cv::Mat M = cv::getRotationMatrix2D(ptCp, degrees, 1.0);
    cv::warpAffine(m->internal(), r->internal(), M, m->internal().size(), cv::INTER_CUBIC);

    return r;
}

QMat *QGeometry::transform(QMat *input, QMat *m){
    if ( !input || !m )
        return nullptr;

    QMat* r = new QMat;
    try {
        cv::transform(input->internal(), *r->internalPtr(), m->internal());
    } catch (cv::Exception& e) {
        THROW_QMLERROR(lv::Exception, e.what(), static_cast<lv::Exception::Code>(e.code), this);
        delete r;
        return nullptr;
    }

    return r;
}

QMat *QGeometry::getPerspectiveTransform(QJSValue src, QJSValue dst)
{
    return getPerspectiveTransform(src.toVariant().toList(), dst.toVariant().toList());
}

/**
 * @brief QGeometry::getPerspectiveTransform
 * @param Four source points (QVariantList with four QPointF's)
 * @param Four destination points (QVariantList with four QPointF's)
 * @return Warp (QMat) that projects the original four points onto the destination ones
 */
QMat *QGeometry::getPerspectiveTransform(QVariantList src, QVariantList dst)
{
    if (src.length() != 4 && dst.length() != 4) return nullptr;
    std::vector<cv::Point2f> srcPts;
    std::vector<cv::Point2f> dstPts;
    for (int i = 0; i < 4; ++i)
    {
        if (src.at(i).userType() != QVariant::PointF) return nullptr;
        srcPts.push_back(cv::Point2f(src.at(i).toPoint().x(), src.at(i).toPoint().y()));
        if (dst.at(i).userType() != QVariant::PointF) return nullptr;
        dstPts.push_back(cv::Point2f(dst.at(i).toPoint().x(), dst.at(i).toPoint().y()));
    }

    return getPerspectiveTransform(srcPts, dstPts);
}

QMat *QGeometry::getPerspectiveTransform(std::vector<cv::Point2f> src, std::vector<cv::Point2f> dst)
{
    return new QMat(new cv::Mat(cv::getPerspectiveTransform(src, dst)), this);
}

QMat *QGeometry::perspectiveProjection(QMat *input, QMat *background, QJSValue points)
{
    if (!background) return nullptr;
    if (!input) return background;
    QVariantList dst = points.toVariant().toList();
    if (dst.empty() || dst.length() != 4) return background;

    try {
        // repack original points
        std::vector<cv::Point2f> dstPts;
        for (int i = 0; i < 4; ++i)
        {
            if (dst.at(i).userType() != QVariant::PointF) return background;
            dstPts.push_back(cv::Point2f(dst.at(i).toPoint().x(), dst.at(i).toPoint().y()));
        }

        // transform input to shape
        std::vector<cv::Point2f> srcPts;
        srcPts.push_back(cv::Point2f(0, 0));
        srcPts.push_back(cv::Point2f(input->dimensions().width(), 0));
        srcPts.push_back(cv::Point2f(input->dimensions().width(),input->dimensions().height()));
        srcPts.push_back(cv::Point2f(0, input->dimensions().height()));

        QMat* transform1 = getPerspectiveTransform(srcPts, dstPts);
        QMat* transformed = warpPerspective(input, transform1, background->dimensions(), INTER_LINEAR, BORDER_CONSTANT);
        delete transform1;

        // create mask to shape
        srcPts.clear();
        srcPts.push_back(cv::Point2f(0, 0));
        srcPts.push_back(cv::Point2f(background->dimensions().width(), 0));
        srcPts.push_back(cv::Point2f(background->dimensions().width(),background->dimensions().height()));
        srcPts.push_back(cv::Point2f(0, background->dimensions().height()));
        QMat* transform2 = getPerspectiveTransform(srcPts, dstPts);
        cv::Mat* m = new cv::Mat(background->dimensions().height(), background->dimensions().width(),
                                 CV_MAKETYPE(background->depth(), 1));
        m->setTo(cv::Scalar(255, 255, 255));
        QMat* white = new QMat(m);
        QMat* mask = warpPerspective(white, transform2, background->dimensions(), INTER_LINEAR, BORDER_CONSTANT);
        delete white;
        delete transform2;


        QTransformations t;
        QMat* result = t.blend(transformed, background, mask);
        delete transformed;
        delete mask;
        return result;

    } catch (cv::Exception& e){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "Geometry: ").jsThrow();
    }

}

QMat *QGeometry::warpPerspective(QMat *input, QMat *transform, QSize size, int flags, int borderMode)
{
    QMat* output = new QMat(
        size.width(),
        size.height(),
        static_cast<QMat::Type>(input->internal().type()),
        input->internal().channels()
    );

    cv::warpPerspective(
        input->internal(),
        output->internal(),
        transform->internal(),
        output->internal().size(),
        flags,
        borderMode,
        cv::Scalar()
    );

    return output;
}

/**
 * @brief QGeometry::applyPerspectiveTransform
 * @param Points to be warped (QVariantList of QPointF's)
 * @param Warp to be applied to the points (QMat*)
 * @return Warped points (QVariantList of QPointF's)
 */
QVariantList QGeometry::applyPerspectiveTransform(QVariantList points, QMat *warp)
{
    std::vector<cv::Point2d> pointsToWarp, warpedPoints;

    if (!warp) return QVariantList();
    for (int i = 0; i < points.size(); ++i)
    {
        if (points.at(i).userType() != QVariant::PointF) return QVariantList();
        auto point = points.at(i).toPointF();
        pointsToWarp.push_back(cv::Point2d(point.x(), point.y()));
    }

    warpedPoints.resize(pointsToWarp.size());

    cv::perspectiveTransform(pointsToWarp, warpedPoints, *warp->internalPtr());

    QVariantList result;
    for (unsigned i = 0; i < warpedPoints.size(); ++i)
    {
        result.push_back(QPointF(warpedPoints[i].x, warpedPoints[i].y));
    }
    return result;
}

void QGeometry::warpTriangles(QMat *src, QMat *dst, QVariantList triangles1, QVariantList triangles2)
{
    if (triangles1.size() != triangles2.size()) return;

    cv::Mat& img1 = *src->internalPtr();
    cv::Mat& img2 = *dst->internalPtr();

    img1.convertTo(img1, CV_32F);
    img2.convertTo(img2, CV_32F);

    for (int i = 0; i < triangles1.size(); ++i)
    {
        std::vector<cv::Point2f> t1, t2;
        if (triangles1.at(i).userType() != QVariant::List ||
            triangles2.at(i).userType() != QVariant::List) return;

        QVariantList t1i = triangles1.at(i).toList();
        QVariantList t2i = triangles2.at(i).toList();
        if (t1i.size() != 3 || t2i.size() != 3) return;
        for (int j=0; j<3; ++j)
        {
            QPointF p1 = t1i.at(j).toPointF();
            QPointF p2 = t2i.at(j).toPointF();
            t1.push_back(cv::Point2f(p1.x(), p1.y()));
            t2.push_back(cv::Point2f(p2.x(), p2.y()));
        }

        cv::Rect r1 = boundingRect(t1);
        cv::Rect r2 = boundingRect(t2);

        // Offset points by left top corner of the respective rectangles
        std::vector<cv::Point2f> t1Rect, t2Rect;
        std::vector<cv::Point> t2RectInt;
        for(int i = 0; i < 3; i++)
        {
            t1Rect.push_back( cv::Point2f( t1[i].x - r1.x, t1[i].y -  r1.y) );
            t2Rect.push_back( cv::Point2f( t2[i].x - r2.x, t2[i].y - r2.y) );
            t2RectInt.push_back( cv::Point(t2[i].x - r2.x, t2[i].y - r2.y) ); // for fillConvexPoly

        }

        // Get mask by filling triangle
        cv::Mat mask = cv::Mat::zeros(r2.height, r2.width, CV_32FC3);
        fillConvexPoly(mask, t2RectInt, cv::Scalar(1.0, 1.0, 1.0), 16, 0);

        // Apply warpImage to small rectangular patches
        cv::Mat img1Rect;
        img1(r1).copyTo(img1Rect);

        cv::Mat img2Rect = cv::Mat::zeros(r2.height, r2.width, img1Rect.type());
        cv::Mat warpMat = cv::getAffineTransform( t1Rect, t2Rect );
        cv::warpAffine( img1Rect, img2Rect, warpMat, img2Rect.size(), cv::INTER_LINEAR, cv::BORDER_REFLECT_101);

        multiply(img2Rect,mask, img2Rect);
        multiply(img2(r2), cv::Scalar(1.0,1.0,1.0) - mask, img2(r2));
        img2(r2) = img2(r2) + img2Rect;
    }


    img1.convertTo(img1, CV_8UC3);
    img2.convertTo(img2, CV_8UC3);

}
