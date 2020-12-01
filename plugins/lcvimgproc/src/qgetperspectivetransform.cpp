#include "qgetperspectivetransform.h"
#include "opencv2/imgproc.hpp"

QGetPerspectiveTransform::QGetPerspectiveTransform(QObject *parent)
    : QObject(parent)
    , m_src(createList())
    , m_dst(createList())
    , m_output(new QMat())
{
}

QGetPerspectiveTransform::~QGetPerspectiveTransform(){
//    delete m_src;
//    delete m_dst;
    delete m_output;
}

void QGetPerspectiveTransform::process(){
    std::vector<cv::Point2f>* srcPts = m_src->dataAs<std::vector<cv::Point2f> >();
    std::vector<cv::Point2f>* dstPts = m_dst->dataAs<std::vector<cv::Point2f> >();
    if ( srcPts->size() == 4 && dstPts->size() == 4 ){
        *m_output->internalPtr() = cv::getPerspectiveTransform(*srcPts, *dstPts);
        emit outputChanged();
    }

}

QVariantList QGetPerspectiveTransform::itemList(lv::QmlVariantList *list){
    std::vector<cv::Point2f>* data = list->dataAs<std::vector<cv::Point2f> >();
    QVariantList result;
    for ( auto it = data->begin(); it != data->end(); ++it ){
        result.append(QPointF(it->x, it->y));
    }

    return result;
}

int QGetPerspectiveTransform::itemCount(lv::QmlVariantList *list){
    std::vector<cv::Point2f>* data = list->dataAs<std::vector<cv::Point2f> >();
    return static_cast<int>(data->size());
}

QVariant QGetPerspectiveTransform::itemAt(lv::QmlVariantList *list, int index){
    std::vector<cv::Point2f>* data = list->dataAs<std::vector<cv::Point2f> >();
    if ( index < (int)data->size() ){
        cv::Point2f& p = data->at(index);
        return QVariant(QPointF(p.x, p.y));
    }
    return QVariant();
}

void QGetPerspectiveTransform::appendItem(lv::QmlVariantList *list, QVariant item){
    std::vector<cv::Point2f>* data = list->dataAs<std::vector<cv::Point2f> >();
    QPointF p = item.toPointF();
    data->push_back(cv::Point2f(p.x(), p.y()));

    QGetPerspectiveTransform* parent = qobject_cast<QGetPerspectiveTransform*>(list->parent());
    if ( parent )
        parent->process();
}

void QGetPerspectiveTransform::removeItemAt(lv::QmlVariantList *list, int index)
{
    std::vector<cv::Point2f>* data = list->dataAs<std::vector<cv::Point2f> >();
    if (index >= data->size()) return;

    data->erase(data->begin()+index);

    QGetPerspectiveTransform* parent = qobject_cast<QGetPerspectiveTransform*>(list->parent());
    if ( parent )
        parent->process();
}

void QGetPerspectiveTransform::clearItems(lv::QmlVariantList *list){
    std::vector<cv::Point2f>* data = list->dataAs<std::vector<cv::Point2f> >();
    data->clear();

    QGetPerspectiveTransform* parent = qobject_cast<QGetPerspectiveTransform*>(list->parent());
    if ( parent )
        parent->process();
}

void QGetPerspectiveTransform::assignItems(lv::QmlVariantList *list, QVariantList items){
    std::vector<cv::Point2f>* data = list->dataAs<std::vector<cv::Point2f> >();
    data->clear();
    for ( auto it = items.begin(); it != items.end(); ++it ){
        QPointF p = it->toPointF();
        data->push_back(cv::Point2f(p.x(), p.y()));
    }

    //TODO: Change with connection
    QGetPerspectiveTransform* parent = qobject_cast<QGetPerspectiveTransform*>(list->parent());
    if ( parent )
        parent->process();
}

void QGetPerspectiveTransform::setSrc(lv::QmlVariantList *src){
    //TODO: Change with connection
    src->setParent(this);
    m_src = src;
    emit srcChanged();

    process();
}

void QGetPerspectiveTransform::setDst(lv::QmlVariantList *dst){
    //TODO: Change with connection
    dst->setParent(this);
    m_dst = dst;
    emit dstChanged();

    process();
}

lv::QmlVariantList *QGetPerspectiveTransform::createList(){
    std::vector<cv::Point2f>* data = new std::vector<cv::Point2f>;
    lv::QmlVariantList* vl = lv::QmlVariantList::create<std::vector<cv::Point2f> >(
        data,
        &QGetPerspectiveTransform::itemList,
        &QGetPerspectiveTransform::itemCount,
        &QGetPerspectiveTransform::itemAt,
        &QGetPerspectiveTransform::appendItem,
        &QGetPerspectiveTransform::removeItemAt,
        &QGetPerspectiveTransform::clearItems,
        this
    );
    vl->setQuickAssign(&QGetPerspectiveTransform::assignItems);

    return vl;
}
