#ifndef QDOCUMENTQMLOBJECT_P_H
#define QDOCUMENTQMLOBJECT_P_H

#include "qdocumentqmlobject.h"
#include "qdocumentqmlinfo.h"
#include "languageutils/fakemetaobject.h"

namespace lcv{

LanguageUtils::FakeMetaObject::Ptr metaObjectFromQmlObject(const QDocumentQmlObject& object){
    LanguageUtils::FakeMetaObject::Ptr fmo(new LanguageUtils::FakeMetaObject);
    fmo->setClassName(object.typeName());
    fmo->setAttachedTypeName(object.typeName());

    for (
        QMap<QString, QString>::const_iterator it = object.memberProperties().begin();
        it != object.memberProperties().end();
        ++it )
    {
        LanguageUtils::FakeMetaProperty fmp(
            it.key(), it.value(), false, true, QDocumentQmlInfo::isObject(object.typeName()), 0
        );
        fmo->addProperty(fmp);
    }

    for (
        QMap<QString, QDocumentQmlObject::FunctionValue>::const_iterator it = object.memberFunctions().begin();
        it != object.memberFunctions().end();
        ++it
    )
    {
        LanguageUtils::FakeMetaMethod fmm(it.value().name);
        fmm.setMethodType(LanguageUtils::FakeMetaMethod::Method);

        for (
            QList<QPair<QString, QString> >::const_iterator argit = it.value().arguments.begin();
            argit != it.value().arguments.end();
            ++argit
        ){
            fmm.addParameter(argit->first, argit->second);
        }

        fmo->addMethod(fmm);
    }

    for (
        QMap<QString, QDocumentQmlObject::FunctionValue>::const_iterator it = object.memberSignals().begin();
        it != object.memberSignals().end();
        ++it
    )
    {
        LanguageUtils::FakeMetaMethod fmm(it.value().name);
        fmm.setMethodType(LanguageUtils::FakeMetaMethod::Signal);

        for (
            QList<QPair<QString, QString> >::const_iterator argit = it.value().arguments.begin();
            argit != it.value().arguments.end();
            ++argit
        ){
            fmm.addParameter(argit->first, argit->second);
        }

        fmo->addMethod(fmm);
    }

    for (
        QMap<QString, QString>::const_iterator it = object.memberSlots().begin();
        it != object.memberSlots().end();
        ++it )
    {
        LanguageUtils::FakeMetaMethod fmm(it.key(), it.value());
        fmm.setMethodType(LanguageUtils::FakeMetaMethod::Slot);
        fmo->addMethod(fmm);
    }

    return fmo;
}

} // namespace

#endif // QDOCUMENTQMLOBJECT_P_H
