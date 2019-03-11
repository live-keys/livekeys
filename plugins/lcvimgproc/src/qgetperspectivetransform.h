#ifndef QGETPERSPECTIVETRANSFORM_H
#define QGETPERSPECTIVETRANSFORM_H

#include <QObject>
#include "live/qmlvariantlist.h"
#include "qmat.h"

class QGetPerspectiveTransform : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::QmlVariantList* src READ src    WRITE setSrc    NOTIFY srcChanged)
    Q_PROPERTY(lv::QmlVariantList* dst READ dst    WRITE setDst    NOTIFY dstChanged)
    Q_PROPERTY(QMat* output            READ output WRITE setOutput NOTIFY outputChanged)

public:
    explicit QGetPerspectiveTransform(QObject *parent = nullptr);
    ~QGetPerspectiveTransform();

    lv::QmlVariantList* src() const;
    lv::QmlVariantList* dst() const;
    QMat* output() const;

    void process();

    static QVariantList itemList(lv::QmlVariantList* list);
    static int itemCount(lv::QmlVariantList* list);
    static QVariant itemAt(lv::QmlVariantList* list, int index);

    static void appendItem(lv::QmlVariantList* list, QVariant item);
    static void removeItemAt(lv::QmlVariantList* list, int index);
    static void clearItems(lv::QmlVariantList* list);
    static void assignItems(lv::QmlVariantList* list, QVariantList items);

signals:
    void srcChanged();
    void dstChanged();
    void outputChanged();

public slots:
    void setSrc(lv::QmlVariantList* src);
    void setDst(lv::QmlVariantList* dst);
    void setOutput(QMat* output);

private:
    lv::QmlVariantList* createList();

    lv::QmlVariantList* m_src;
    lv::QmlVariantList* m_dst;
    QMat*               m_output;
};

inline lv::QmlVariantList *QGetPerspectiveTransform::src() const{
    return m_src;
}

inline lv::QmlVariantList *QGetPerspectiveTransform::dst() const{
    return m_dst;
}

inline QMat *QGetPerspectiveTransform::output() const{
    return m_output;
}

inline void QGetPerspectiveTransform::setOutput(QMat *output){
    m_output = output;
    emit outputChanged();
}

#endif // QGETPERSPECTIVETRANSFORM_H
