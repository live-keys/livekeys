#include "qshapedescriptors.h"
#include <opencv2/opencv.hpp>
#include <QPointF>
#include "qmat.h"

QShapeDescriptors::QShapeDescriptors(QObject *parent) : QObject(parent)
{

}

/**
 * @brief QShapePredictors::convexHullIndices
 * @param List of 2D points (QVariantList of QPointF's)
 * @return Indices of those points that are on the convex hull
 *         of the set (QVariantList of ints)
 */
QVariantList QShapeDescriptors::convexHullIndices(QVariantList points)
{

    std::vector<cv::Point2f> points2;
    if (points.size() == 0) return QVariantList();
    for (int i = 0; i < points.size(); ++i)
    {
        if (points.at(i).userType() != QVariant::PointF) return QVariantList();
        auto point = points.at(i).toPointF();
        points2.push_back(cv::Point2f(point.x(), point.y()));
    }
    std::vector<int> hullIndex;
    cv::convexHull(points2, hullIndex, false, false);

    QVariantList result;
    for (unsigned i = 0; i < hullIndex.size(); ++i)
        result.push_back(hullIndex[i]);

    return result;
}

/**
 * @brief QShapeDescriptors::DelaunayTriangles
 * @param Size of rectangle containing points (QSize)
 * @param List of points in given rectangle (QVariantList of QPointF's)
 * @return List of int triplets containing indices of triangles
 *         (QVariantList of QVariantLists containing three ints)
 *
 * Number of triangles is always n-2 for a given set of points of size n, since we're
 * always passing a convex set of points
 */
QVariantList QShapeDescriptors::DelaunayTriangles(QSize rectSize, QVariantList points)
{
    if (rectSize.isNull()) return QVariantList();

    cv::Rect rect(0,0,rectSize.width(), rectSize.height());
    cv::Subdiv2D subdiv(rect);

    // Insert points into subdiv
    for(int i = 0; i < points.size(); ++i){
        QPointF point = points.at(i).toPointF();
        subdiv.insert(cv::Point2f(point.x(), point.y()));
    }

    std::vector<cv::Vec6f> triangleList;
    subdiv.getTriangleList(triangleList);

    QVariantList result;

    for( size_t i = 0; i < triangleList.size(); i++ )
    {
        QVariantList indices;
        cv::Vec6f t = triangleList[i];
        std::vector<cv::Point2f> pt(3);
        pt[0] = cv::Point2f(t[0], t[1]);
        pt[1] = cv::Point2f(t[2], t[3]);
        pt[2] = cv::Point2f(t[4], t[5]);

        if ( rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2])){
            for(int j = 0; j < 3; j++)
            {
                for(int k = 0; k < points.size(); k++)
                {
                    QPointF point = points.at(k).toPointF();
                    if(abs(pt[j].x - point.x()) < 1.0 && abs(pt[j].y - point.y()) < 1.0)
                    {
                        indices.push_back(k);
                        break;
                    }
                }
            }

            result.push_back(indices);
        }
    }


    return result;
}

/**
 * @brief QShapeDescriptors::getMaskFromHullPoints
 * @param Hull points
 * @param Mask matrix size
 * @param Mask matrix depth
 * @return
 */
QMat *QShapeDescriptors::getMaskFromHullPoints(QVariantList points, QSize size, int depth)
{
    std::vector<cv::Point> hull8U;
    if (points.size() == 0) return nullptr;
    for(int i = 0; i < points.size(); i++)
    {
        QPointF point = points.at(i).toPointF();
        cv::Point pt(point.x(), point.y());
        hull8U.push_back(pt);
    }


    cv::Mat nu = cv::Mat::zeros(size.height(), size.width(), depth);

    cv::fillConvexPoly(nu, &hull8U[0], hull8U.size(), cv::Scalar(255,255,255));

    QMat* mask = new QMat(new cv::Mat(nu));
    return mask;
}
