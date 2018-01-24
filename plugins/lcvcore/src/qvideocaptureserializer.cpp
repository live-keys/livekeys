#include "qvideocaptureserializer.h"
#include "qvideocapture.h"

#include <QJSValueIterator>

#include "live/documentqmlinfo.h"
#include "live/documentqmlfragment.h"
#include "live/projectdocument.h"
#include "live/projectfile.h"

QVideoCaptureSerializer::QVideoCaptureSerializer(QObject *parent)
    : lv::AbstractCodeSerializer(parent)
{
}

QVideoCaptureSerializer::~QVideoCaptureSerializer(){
}

QString QVideoCaptureSerializer::toCode(const QVariant &value, const lv::DocumentEditFragment *channel){
    const lv::DocumentQmlFragment* qmlfragment = static_cast<const lv::DocumentQmlFragment*>(channel);
    int valuePosition = qmlfragment->declaration()->valuePosition();
    int valueLength   = qmlfragment->declaration()->valueLength();
    QString source = qmlfragment->declaration()->document()->content().mid(valuePosition, valueLength);

    lv::DocumentQmlInfo::Ptr docinfo = lv::DocumentQmlInfo::create(qmlfragment->declaration()->document()->file()->path());
    if ( !docinfo->parse(source) )
        return "";

    lv::DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();
    lv::DocumentQmlValueObjects::RangeObject* root = objects->root();

    QObject* ob = value.value<QObject*>();
    if ( !ob )
        return "";

    QVideoCapture* capob = qobject_cast<QVideoCapture*>(ob);
    if ( !capob )
        return "";

    bool isPaused    = capob->paused();
    int currentFrame = capob->currentFrame();
    qreal fps        = capob->fps();


    for ( auto it = root->properties.begin(); it != root->properties.end(); ++it ){
        lv::DocumentQmlValueObjects::RangeProperty* p = *it;
        QString propertyName = source.mid(p->begin, p->propertyEnd - p->begin);
        if ( propertyName == "paused" ){
            source.replace(p->valueBegin, p->end - p->valueBegin, isPaused ? "true" : "false");
        } else if ( propertyName == "currentFrame" ){
            source.replace(p->valueBegin, p->end - p->valueBegin, QString::number(currentFrame));
        } else if ( propertyName == "fps" ){
            source.replace(p->valueBegin, p->end - p->valueBegin, QString::number(fps));
        }
    }

    return source;
}

QVariant QVideoCaptureSerializer::fromCode(const QString &, const lv::DocumentEditFragment *channel){
    const lv::DocumentQmlFragment* qmlfragment = static_cast<const lv::DocumentQmlFragment*>(channel);
    if ( !qmlfragment )
        return QVariant();

    QQmlListReference ppref = qvariant_cast<QQmlListReference>(qmlfragment->property().read());
    return QVariant::fromValue(ppref.at(qmlfragment->listIndex()));
}
