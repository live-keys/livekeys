/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtQml/qqmlengine.h>
#include <QtQml/private/qqmlmetatype_p.h>
#include <QtQml/private/qqmlopenmetaobject_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquickpincharea_p.h>

#include <QtGui/QGuiApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QSet>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QProcess>
#include <QtCore/private/qobject_p.h>
#include <QtCore/private/qmetaobject_p.h>

#include <iostream>
#include <algorithm>

#include "qmlstreamwriter.h"

#ifdef QT_SIMULATOR
#include <QtGui/private/qsimulatorconnection_p.h>
#endif

#ifdef Q_OS_UNIX
#include <signal.h>
#endif
#ifdef Q_OS_WIN
#  if !defined(Q_CC_MINGW)
#    include <crtdbg.h>
#  endif
#include <qt_windows.h>
#endif

QString pluginImportPath;
bool verbose = false;
bool creatable = true;

QString currentProperty;
QString inObjectInstantiation;

static QString enquote(const QString &string)
{
    QString s = string;
    return QString("\"%1\"").arg(s.replace(QLatin1Char('\\'), QLatin1String("\\\\"))
                                 .replace(QLatin1Char('"'),QLatin1String("\\\"")));
}

void collectReachableMetaObjects(const QMetaObject *meta, QSet<const QMetaObject *> *metas, bool extended = false)
{
    if (! meta || metas->contains(meta))
        return;

    // dynamic meta objects can break things badly
    // but extended types are usually fine
    const QMetaObjectPrivate *mop = reinterpret_cast<const QMetaObjectPrivate *>(meta->d.data);
    if (extended || !(mop->flags & DynamicMetaObject))
        metas->insert(meta);

    collectReachableMetaObjects(meta->superClass(), metas);
}

void collectReachableMetaObjects(QObject *object, QSet<const QMetaObject *> *metas)
{
    if (! object)
        return;

    const QMetaObject *meta = object->metaObject();
    if (verbose)
        std::cerr << "Processing object" << qPrintable( meta->className() ) << std::endl;
    collectReachableMetaObjects(meta, metas);

    for (int index = 0; index < meta->propertyCount(); ++index) {
        QMetaProperty prop = meta->property(index);
        if (QQmlMetaType::isQObject(prop.userType())) {
            if (verbose)
                std::cerr << "  Processing property" << qPrintable( prop.name() ) << std::endl;
            currentProperty = QString("%1::%2").arg(meta->className(), prop.name());

            // if the property was not initialized during construction,
            // accessing a member of oo is going to cause a segmentation fault
            QObject *oo = QQmlMetaType::toQObject(prop.read(object));
            if (oo && !metas->contains(oo->metaObject()))
                collectReachableMetaObjects(oo, metas);
            currentProperty.clear();
        }
    }
}

void collectReachableMetaObjects(const QQmlType *ty, QSet<const QMetaObject *> *metas)
{
    collectReachableMetaObjects(ty->metaObject(), metas, ty->isExtendedType());
    if (ty->attachedPropertiesType())
        collectReachableMetaObjects(ty->attachedPropertiesType(), metas);
}

/* We want to add the MetaObject for 'Qt' to the list, this is a
   simple way to access it.
*/
class FriendlyQObject: public QObject
{
public:
    static const QMetaObject *qtMeta() { return &staticQtMetaObject; }
};

/* When we dump a QMetaObject, we want to list all the types it is exported as.
   To do this, we need to find the QQmlTypes associated with this
   QMetaObject.
*/
static QHash<QByteArray, QSet<const QQmlType *> > qmlTypesByCppName;

// No different versioning possible for a composite type.
static QMap<QString, const QQmlType * > qmlTypesByCompositeName;

static QHash<QByteArray, QByteArray> cppToId;

/* Takes a C++ type name, such as Qt::LayoutDirection or QString and
   maps it to how it should appear in the description file.

   These names need to be unique globally, so we don't change the C++ symbol's
   name much. It is mostly used to for explicit translations such as
   QString->string and translations for extended QML objects.
*/
QByteArray convertToId(const QByteArray &cppName)
{
    return cppToId.value(cppName, cppName);
}

QByteArray convertToId(const QMetaObject *mo)
{
    QByteArray className(mo->className());
    if (!className.isEmpty())
        return convertToId(className);

    // likely a metaobject generated for an extended qml object
    if (mo->superClass()) {
        className = convertToId(mo->superClass());
        className.append("_extended");
        return className;
    }

    static QHash<const QMetaObject *, QByteArray> generatedNames;
    className = generatedNames.value(mo);
    if (!className.isEmpty())
        return className;

    std::cerr << "Found a QMetaObject without a className, generating a random name" << std::endl;
    className = QByteArray("error-unknown-name-");
    className.append(QByteArray::number(generatedNames.size()));
    generatedNames.insert(mo, className);
    return className;
}


// Collect all metaobjects for types registered with qmlRegisterType() without parameters
void collectReachableMetaObjectsWithoutQmlName( QSet<const QMetaObject *>& metas ) {
    foreach (const QQmlType *ty, QQmlMetaType::qmlAllTypes()) {
        if ( ! metas.contains(ty->metaObject()) ) {
            if (!ty->isComposite()) {
                collectReachableMetaObjects(ty, &metas);
            } else {
                qmlTypesByCompositeName[ty->elementName()] = ty;
            }
       }
    }
}

QSet<const QMetaObject *> collectReachableMetaObjects(QQmlEngine *engine,
                                                      QSet<const QMetaObject *> &noncreatables,
                                                      QSet<const QMetaObject *> &singletons,
                                                      const QList<QQmlType *> &skip = QList<QQmlType *>())
{
    QSet<const QMetaObject *> metas;
    metas.insert(FriendlyQObject::qtMeta());

    QHash<QByteArray, QSet<QByteArray> > extensions;
    foreach (const QQmlType *ty, QQmlMetaType::qmlTypes()) {
        if (!ty->isCreatable())
            noncreatables.insert(ty->metaObject());
        if (ty->isSingleton())
            singletons.insert(ty->metaObject());
        if (!ty->isComposite()) {
            qmlTypesByCppName[ty->metaObject()->className()].insert(ty);
            if (ty->isExtendedType())
                extensions[ty->typeName()].insert(ty->metaObject()->className());
            collectReachableMetaObjects(ty, &metas);
        } else {
            qmlTypesByCompositeName[ty->elementName()] = ty;
        }
    }

    // Adjust exports of the base object if there are extensions.
    // For each export of a base object there can be a single extension object overriding it.
    // Example: QDeclarativeGraphicsWidget overrides the QtQuick/QGraphicsWidget export
    //          of QGraphicsWidget.
    foreach (const QByteArray &baseCpp, extensions.keys()) {
        QSet<const QQmlType *> baseExports = qmlTypesByCppName.value(baseCpp);

        const QSet<QByteArray> extensionCppNames = extensions.value(baseCpp);
        foreach (const QByteArray &extensionCppName, extensionCppNames) {
            const QSet<const QQmlType *> extensionExports = qmlTypesByCppName.value(extensionCppName);

            // remove extension exports from base imports
            // unfortunately the QQmlType pointers don't match, so can't use QSet::subtract
            QSet<const QQmlType *> newBaseExports;
            foreach (const QQmlType *baseExport, baseExports) {
                bool match = false;
                foreach (const QQmlType *extensionExport, extensionExports) {
                    if (baseExport->qmlTypeName() == extensionExport->qmlTypeName()
                            && baseExport->majorVersion() == extensionExport->majorVersion()
                            && baseExport->minorVersion() == extensionExport->minorVersion()) {
                        match = true;
                        break;
                    }
                }
                if (!match)
                    newBaseExports.insert(baseExport);
            }
            baseExports = newBaseExports;
        }
        qmlTypesByCppName[baseCpp] = baseExports;
    }

    if (creatable) {
        // find even more QMetaObjects by instantiating QML types and running
        // over the instances
        foreach (QQmlType *ty, QQmlMetaType::qmlTypes()) {
            if (skip.contains(ty))
                continue;
            if (ty->isExtendedType())
                continue;
            if (!ty->isCreatable())
                continue;
            if (ty->typeName() == "QQmlComponent")
                continue;

            QString tyName = ty->qmlTypeName();
            tyName = tyName.mid(tyName.lastIndexOf(QLatin1Char('/')) + 1);
            if (tyName.isEmpty())
                continue;

            inObjectInstantiation = tyName;
            QObject *object = 0;

            if (ty->isSingleton()) {
                QQmlType::SingletonInstanceInfo *siinfo = ty->singletonInstanceInfo();
                if (!siinfo) {
                    std::cerr << "Internal error, " << qPrintable(tyName)
                              << "(" << qPrintable( QString::fromUtf8(ty->typeName()) ) << ")"
                              << " is singleton, but has no singletonInstanceInfo" << std::endl;
                    continue;
                }
                if (siinfo->qobjectCallback) {
                    if (verbose)
                        std::cerr << "Trying to get singleton for " << qPrintable(tyName)
                                  << " (" << qPrintable( siinfo->typeName )  << ")" << std::endl;
                    siinfo->init(engine);
                    collectReachableMetaObjects(object, &metas);
                    object = siinfo->qobjectApi(engine);
                } else {
                    inObjectInstantiation.clear();
                    continue; // we don't handle QJSValue singleton types.
                }
            } else {
                if (verbose)
                    std::cerr << "Trying to create object " << qPrintable( tyName )
                              << " (" << qPrintable( QString::fromUtf8(ty->typeName()) )  << ")" << std::endl;
                object = ty->create();
            }

            inObjectInstantiation.clear();

            if (object) {
                if (verbose)
                    std::cerr << "Got " << qPrintable( tyName )
                              << " (" << qPrintable( QString::fromUtf8(ty->typeName()) ) << ")" << std::endl;
                collectReachableMetaObjects(object, &metas);
                object->deleteLater();
            } else {
                std::cerr << "Could not create" << qPrintable(tyName) << std::endl;
            }
        }
    }

    collectReachableMetaObjectsWithoutQmlName(metas);

    return metas;
}

class KnownAttributes {
    QHash<QByteArray, int> m_properties;
    QHash<QByteArray, QHash<int, int> > m_methods;
public:
    bool knownMethod(const QByteArray &name, int nArgs, int revision)
    {
        if (m_methods.contains(name)) {
            QHash<int, int> overloads = m_methods.value(name);
            if (overloads.contains(nArgs) && overloads.value(nArgs) <= revision)
                return true;
        }
        m_methods[name][nArgs] = revision;
        return false;
    }

    bool knownProperty(const QByteArray &name, int revision)
    {
        if (m_properties.contains(name) && m_properties.value(name) <= revision)
            return true;
        m_properties[name] = revision;
        return false;
    }
};

class Dumper
{
    QmlStreamWriter *qml;
    QString relocatableModuleUri;

public:
    Dumper(QmlStreamWriter *qml) : qml(qml) {}

    void setRelocatableModuleUri(const QString &uri)
    {
        relocatableModuleUri = uri;
    }

    const QString getExportString(QString qmlTyName, int majorVersion, int minorVersion)
    {
        if (qmlTyName.startsWith(relocatableModuleUri + QLatin1Char('/'))) {
            qmlTyName.remove(0, relocatableModuleUri.size() + 1);
        }
        if (qmlTyName.startsWith("./")) {
            qmlTyName.remove(0, 2);
        }
        if (qmlTyName.startsWith("/")) {
            qmlTyName.remove(0, 1);
        }
        const QString exportString = enquote(
                    QString("%1 %2.%3").arg(
                        qmlTyName,
                        QString::number(majorVersion),
                        QString::number(minorVersion)));
        return exportString;
    }

    void writeMetaContent(const QMetaObject *meta, KnownAttributes *knownAttributes = 0)
    {
        QSet<QString> implicitSignals;
        for (int index = meta->propertyOffset(); index < meta->propertyCount(); ++index) {
            const QMetaProperty &property = meta->property(index);
            dump(property, knownAttributes);
            if (knownAttributes)
                knownAttributes->knownMethod(QByteArray(property.name()).append("Changed"),
                                             0, property.revision());
            implicitSignals.insert(QString("%1Changed").arg(QString::fromUtf8(property.name())));
        }

        if (meta == &QObject::staticMetaObject) {
            // for QObject, hide deleteLater() and onDestroyed
            for (int index = meta->methodOffset(); index < meta->methodCount(); ++index) {
                QMetaMethod method = meta->method(index);
                QByteArray signature = method.methodSignature();
                if (signature == QByteArrayLiteral("destroyed(QObject*)")
                        || signature == QByteArrayLiteral("destroyed()")
                        || signature == QByteArrayLiteral("deleteLater()"))
                    continue;
                dump(method, implicitSignals, knownAttributes);
            }

            // and add toString(), destroy() and destroy(int)
            if (!knownAttributes || !knownAttributes->knownMethod(QByteArray("toString"), 0, 0)) {
                qml->writeStartObject(QLatin1String("Method"));
                qml->writeScriptBinding(QLatin1String("name"), enquote(QLatin1String("toString")));
                qml->writeEndObject();
            }
            if (!knownAttributes || !knownAttributes->knownMethod(QByteArray("destroy"), 0, 0)) {
                qml->writeStartObject(QLatin1String("Method"));
                qml->writeScriptBinding(QLatin1String("name"), enquote(QLatin1String("destroy")));
                qml->writeEndObject();
            }
            if (!knownAttributes || !knownAttributes->knownMethod(QByteArray("destroy"), 1, 0)) {
                qml->writeStartObject(QLatin1String("Method"));
                qml->writeScriptBinding(QLatin1String("name"), enquote(QLatin1String("destroy")));
                qml->writeStartObject(QLatin1String("Parameter"));
                qml->writeScriptBinding(QLatin1String("name"), enquote(QLatin1String("delay")));
                qml->writeScriptBinding(QLatin1String("type"), enquote(QLatin1String("int")));
                qml->writeEndObject();
                qml->writeEndObject();
            }
        } else {
            for (int index = meta->methodOffset(); index < meta->methodCount(); ++index)
                dump(meta->method(index), implicitSignals, knownAttributes);
        }
    }

    QString getPrototypeNameForCompositeType(const QMetaObject *metaObject, QSet<QByteArray> &defaultReachableNames,
                                             QList<const QMetaObject *> *objectsToMerge)
    {
        QString prototypeName;
        if (!defaultReachableNames.contains(metaObject->className())) {
            // dynamic meta objects can break things badly
            // but extended types are usually fine
            const QMetaObjectPrivate *mop = reinterpret_cast<const QMetaObjectPrivate *>(metaObject->d.data);
            if (!(mop->flags & DynamicMetaObject) && objectsToMerge
                    && !objectsToMerge->contains(metaObject))
                objectsToMerge->append(metaObject);
            const QMetaObject *superMetaObject = metaObject->superClass();
            if (!superMetaObject)
                prototypeName = "QObject";
            else
                prototypeName = getPrototypeNameForCompositeType(
                            superMetaObject, defaultReachableNames, objectsToMerge);
        } else {
            prototypeName = convertToId(metaObject->className());
        }
        return prototypeName;
    }

    void dumpComposite(QQmlEngine *engine, const QQmlType *compositeType, QSet<QByteArray> &defaultReachableNames)
    {
        QQmlComponent e(engine, compositeType->sourceUrl());
        if (!e.isReady()) {
            std::cerr << "WARNING: skipping module " << compositeType->elementName().toStdString()
                      << std::endl << e.errorString().toStdString() << std::endl;
            return;
        }

        QObject *object = e.create();

        if (!object)
            return;

        qml->writeStartObject("Component");

        const QMetaObject *mainMeta = object->metaObject();

        QList<const QMetaObject *> objectsToMerge;
        KnownAttributes knownAttributes;
        // Get C++ base class name for the composite type
        QString prototypeName = getPrototypeNameForCompositeType(mainMeta, defaultReachableNames,
                                                                 &objectsToMerge);
        qml->writeScriptBinding(QLatin1String("prototype"), enquote(prototypeName));

        QString qmlTyName = compositeType->qmlTypeName();
        // name should be unique
        qml->writeScriptBinding(QLatin1String("name"), enquote(qmlTyName));
        const QString exportString = getExportString(qmlTyName, compositeType->majorVersion(), compositeType->minorVersion());
        qml->writeArrayBinding(QLatin1String("exports"), QStringList() << exportString);
        qml->writeArrayBinding(QLatin1String("exportMetaObjectRevisions"), QStringList() << QString::number(compositeType->minorVersion()));
        qml->writeBooleanBinding(QLatin1String("isComposite"), true);

        if (compositeType->isSingleton()) {
            qml->writeBooleanBinding(QLatin1String("isCreatable"), false);
            qml->writeBooleanBinding(QLatin1String("isSingleton"), true);
        }

        for (int index = mainMeta->classInfoCount() - 1 ; index >= 0 ; --index) {
            QMetaClassInfo classInfo = mainMeta->classInfo(index);
            if (QLatin1String(classInfo.name()) == QLatin1String("DefaultProperty")) {
                qml->writeScriptBinding(QLatin1String("defaultProperty"), enquote(QLatin1String(classInfo.value())));
                break;
            }
        }

        foreach (const QMetaObject *meta, objectsToMerge)
            writeMetaContent(meta, &knownAttributes);

        qml->writeEndObject();
    }

    void dump(const QMetaObject *meta, bool isUncreatable, bool isSingleton)
    {
        qml->writeStartObject("Component");

        QByteArray id = convertToId(meta);
        qml->writeScriptBinding(QLatin1String("name"), enquote(id));

        for (int index = meta->classInfoCount() - 1 ; index >= 0 ; --index) {
            QMetaClassInfo classInfo = meta->classInfo(index);
            if (QLatin1String(classInfo.name()) == QLatin1String("DefaultProperty")) {
                qml->writeScriptBinding(QLatin1String("defaultProperty"), enquote(QLatin1String(classInfo.value())));
                break;
            }
        }

        if (meta->superClass())
            qml->writeScriptBinding(QLatin1String("prototype"), enquote(convertToId(meta->superClass())));

        QSet<const QQmlType *> qmlTypes = qmlTypesByCppName.value(meta->className());
        if (!qmlTypes.isEmpty()) {
            QHash<QString, const QQmlType *> exports;

            foreach (const QQmlType *qmlTy, qmlTypes) {
                const QString exportString = getExportString(qmlTy->qmlTypeName(), qmlTy->majorVersion(), qmlTy->minorVersion());
                exports.insert(exportString, qmlTy);
            }

            // ensure exports are sorted and don't change order when the plugin is dumped again
            QStringList exportStrings = exports.keys();
            std::sort(exportStrings.begin(), exportStrings.end());
            qml->writeArrayBinding(QLatin1String("exports"), exportStrings);

            if (isUncreatable)
                qml->writeBooleanBinding(QLatin1String("isCreatable"), false);

            if (isSingleton)
                qml->writeBooleanBinding(QLatin1String("isSingleton"), true);

            // write meta object revisions
            QStringList metaObjectRevisions;
            foreach (const QString &exportString, exportStrings) {
                int metaObjectRevision = exports[exportString]->metaObjectRevision();
                metaObjectRevisions += QString::number(metaObjectRevision);
            }
            qml->writeArrayBinding(QLatin1String("exportMetaObjectRevisions"), metaObjectRevisions);

            if (const QMetaObject *attachedType = (*qmlTypes.begin())->attachedPropertiesType()) {
                // Can happen when a type is registered that returns itself as attachedPropertiesType()
                // because there is no creatable type to attach to.
                if (attachedType != meta) {
                    qml->writeScriptBinding(QLatin1String("attachedType"), enquote(
                                                convertToId(attachedType)));
                }
            }
        }

        for (int index = meta->enumeratorOffset(); index < meta->enumeratorCount(); ++index)
            dump(meta->enumerator(index));

        writeMetaContent(meta);

        qml->writeEndObject();
    }

    void writeEasingCurve()
    {
        qml->writeStartObject(QLatin1String("Component"));
        qml->writeScriptBinding(QLatin1String("name"), enquote(QLatin1String("QEasingCurve")));
        qml->writeScriptBinding(QLatin1String("prototype"), enquote(QLatin1String("QQmlEasingValueType")));
        qml->writeEndObject();
    }

private:

    /* Removes pointer and list annotations from a type name, returning
       what was removed in isList and isPointer
    */
    static void removePointerAndList(QByteArray *typeName, bool *isList, bool *isPointer)
    {
        static QByteArray declListPrefix = "QQmlListProperty<";

        if (typeName->endsWith('*')) {
            *isPointer = true;
            typeName->truncate(typeName->length() - 1);
            removePointerAndList(typeName, isList, isPointer);
        } else if (typeName->startsWith(declListPrefix)) {
            *isList = true;
            typeName->truncate(typeName->length() - 1); // get rid of the suffix '>'
            *typeName = typeName->mid(declListPrefix.size());
            removePointerAndList(typeName, isList, isPointer);
        }

        *typeName = convertToId(*typeName);
    }

    void writeTypeProperties(QByteArray typeName, bool isWritable)
    {
        bool isList = false, isPointer = false;
        removePointerAndList(&typeName, &isList, &isPointer);

        qml->writeScriptBinding(QLatin1String("type"), enquote(typeName));
        if (isList)
            qml->writeScriptBinding(QLatin1String("isList"), QLatin1String("true"));
        if (!isWritable)
            qml->writeScriptBinding(QLatin1String("isReadonly"), QLatin1String("true"));
        if (isPointer)
            qml->writeScriptBinding(QLatin1String("isPointer"), QLatin1String("true"));
    }

    void dump(const QMetaProperty &prop, KnownAttributes *knownAttributes = 0)
    {
        int revision = prop.revision();
        QByteArray propName = prop.name();
        if (knownAttributes && knownAttributes->knownProperty(propName, revision))
            return;
        qml->writeStartObject("Property");
        qml->writeScriptBinding(QLatin1String("name"), enquote(QString::fromUtf8(prop.name())));
        if (revision)
            qml->writeScriptBinding(QLatin1String("revision"), QString::number(revision));
        writeTypeProperties(prop.typeName(), prop.isWritable());

        qml->writeEndObject();
    }

    void dump(const QMetaMethod &meth, const QSet<QString> &implicitSignals,
              KnownAttributes *knownAttributes = 0)
    {
        if (meth.methodType() == QMetaMethod::Signal) {
            if (meth.access() != QMetaMethod::Public)
                return; // nothing to do.
        } else if (meth.access() != QMetaMethod::Public) {
            return; // nothing to do.
        }

        QByteArray name = meth.name();
        const QString typeName = convertToId(meth.typeName());

        if (implicitSignals.contains(name)
                && !meth.revision()
                && meth.methodType() == QMetaMethod::Signal
                && meth.parameterNames().isEmpty()
                && typeName == QLatin1String("void")) {
            // don't mention implicit signals
            return;
        }

        int revision = meth.revision();
        if (knownAttributes && knownAttributes->knownMethod(name, meth.parameterNames().size(), revision))
            return;
        if (meth.methodType() == QMetaMethod::Signal)
            qml->writeStartObject(QLatin1String("Signal"));
        else
            qml->writeStartObject(QLatin1String("Method"));

        qml->writeScriptBinding(QLatin1String("name"), enquote(name));

        if (revision)
            qml->writeScriptBinding(QLatin1String("revision"), QString::number(revision));

        if (typeName != QLatin1String("void"))
            qml->writeScriptBinding(QLatin1String("type"), enquote(typeName));

        for (int i = 0; i < meth.parameterTypes().size(); ++i) {
            QByteArray argName = meth.parameterNames().at(i);

            qml->writeStartObject(QLatin1String("Parameter"));
            if (! argName.isEmpty())
                qml->writeScriptBinding(QLatin1String("name"), enquote(argName));
            writeTypeProperties(meth.parameterTypes().at(i), true);
            qml->writeEndObject();
        }

        qml->writeEndObject();
    }

    void dump(const QMetaEnum &e)
    {
        qml->writeStartObject(QLatin1String("Enum"));
        qml->writeScriptBinding(QLatin1String("name"), enquote(QString::fromUtf8(e.name())));

        QList<QPair<QString, QString> > namesValues;
        for (int index = 0; index < e.keyCount(); ++index) {
            namesValues.append(qMakePair(enquote(QString::fromUtf8(e.key(index))), QString::number(e.value(index))));
        }

        qml->writeScriptObjectLiteralBinding(QLatin1String("values"), namesValues);
        qml->writeEndObject();
    }
};

enum ExitCode {
    EXIT_INVALIDARGUMENTS = 1,
    EXIT_SEGV = 2,
    EXIT_IMPORTERROR = 3
};

#ifdef Q_OS_UNIX
void sigSegvHandler(int) {
    fprintf(stderr, "Error: SEGV\n");
    if (!currentProperty.isEmpty())
        fprintf(stderr, "While processing the property '%s', which probably has uninitialized data.\n", currentProperty.toLatin1().constData());
    if (!inObjectInstantiation.isEmpty())
        fprintf(stderr, "While instantiating the object '%s'.\n", inObjectInstantiation.toLatin1().constData());
    exit(EXIT_SEGV);
}
#endif

void printUsage(const QString &appName)
{
    std::cerr << qPrintable(QString(
                                 "Usage: %1 [-v] [-noinstantiate] [-defaultplatform] [-[non]relocatable] [-dependencies <dependencies.json>] module.uri version [module/import/path]\n"
                                 "       %1 [-v] [-noinstantiate] -path path/to/qmldir/directory [version]\n"
                                 "       %1 [-v] -builtins\n"
                                 "Example: %1 Qt.labs.folderlistmodel 2.0 /home/user/dev/qt-install/imports").arg(
                                 appName)) << std::endl;
}

static bool readDependenciesData(QString dependenciesFile, const QByteArray &fileData,
                                 QStringList *dependencies, const QStringList &urisToSkip) {
    if (verbose) {
        std::cerr << "parsing "
                  << qPrintable( dependenciesFile ) << " skipping";
        foreach (const QString &uriToSkip, urisToSkip)
            std::cerr << " "  << qPrintable(uriToSkip);
        std::cerr << std::endl;
    }
    QJsonParseError parseError;
    parseError.error = QJsonParseError::NoError;
    QJsonDocument doc = QJsonDocument::fromJson(fileData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        std::cerr << "Error parsing dependencies file " << dependenciesFile.toStdString()
                  << ":" << parseError.errorString().toStdString() << " at " << parseError.offset
                  << std::endl;
        return false;
    }
    if (doc.isArray()) {
        QStringList requiredKeys = QStringList() << QStringLiteral("name")
                                                 << QStringLiteral("type")
                                                 << QStringLiteral("version");
        foreach (const QJsonValue &dep, doc.array()) {
            if (dep.isObject()) {
                QJsonObject obj = dep.toObject();
                foreach (const QString &requiredKey, requiredKeys)
                    if (!obj.contains(requiredKey) || obj.value(requiredKey).isString())
                        continue;
                if (obj.value(QStringLiteral("type")).toString() != QLatin1String("module"))
                    continue;
                QString name = obj.value((QStringLiteral("name"))).toString();
                QString version = obj.value(QStringLiteral("version")).toString();
                if (name.isEmpty() || urisToSkip.contains(name) || version.isEmpty())
                    continue;
                if (name.endsWith(QLatin1String("Private"))) {
                    if (verbose)
                        std::cerr << "skipping private dependecy "
                                  << qPrintable( name ) << " "  << qPrintable(version) << std::endl;
                    continue;
                }
                if (verbose)
                    std::cerr << "appending dependency "
                              << qPrintable( name ) << " "  << qPrintable(version) << std::endl;
                dependencies->append(name + QLatin1Char(' ')+version);
            }
        }
    } else {
        std::cerr << "Error parsing dependencies file " << dependenciesFile.toStdString()
                  << ": expected an array" << std::endl;
        return false;
    }
    return true;
}

static bool readDependenciesFile(QString dependenciesFile, QStringList *dependencies,
                                const QStringList &urisToSkip) {
    if (!QFileInfo(dependenciesFile).exists()) {
        std::cerr << "non existing dependencies file " << dependenciesFile.toStdString()
                  << std::endl;
        return false;
    }
    QFile f(dependenciesFile);
    if (!f.open(QFileDevice::ReadOnly)) {
        std::cerr << "non existing dependencies file " << dependenciesFile.toStdString()
                  << ", " << f.errorString().toStdString() << std::endl;
        return false;
    }
    QByteArray fileData = f.readAll();
    return readDependenciesData(dependenciesFile, fileData, dependencies, urisToSkip);
}

static bool getDependencies(const QQmlEngine &engine, const QString &pluginImportUri,
                            const QString &pluginImportVersion, QStringList *dependencies)
{
    QFileInfo selfExe(QCoreApplication::applicationFilePath());
    QString command = selfExe.absoluteDir().filePath(QLatin1String("qmlimportscanner")
                                                     + selfExe.suffix());

    QStringList commandArgs = QStringList()
            << QLatin1String("-qmlFiles")
            << QLatin1String("-");
    foreach (const QString &path, engine.importPathList())
        commandArgs << QLatin1String("-importPath") << path;

    QProcess importScanner;
    importScanner.start(command, commandArgs, QProcess::ReadWrite);
    if (!importScanner.waitForStarted())
        return false;

    importScanner.write("import ");
    importScanner.write(pluginImportUri.toUtf8());
    importScanner.write(" ");
    importScanner.write(pluginImportVersion.toUtf8());
    importScanner.write("\nQtObject{}\n");
    importScanner.closeWriteChannel();

    if (!importScanner.waitForFinished()) {
        std::cerr << "failure to start " << qPrintable(command);
        foreach (const QString &arg, commandArgs)
            std::cerr << " " << qPrintable(arg);
        std::cerr << std::endl;
        return false;
    }
    QByteArray depencenciesData = importScanner.readAllStandardOutput();
    if (!readDependenciesData(QLatin1String("<outputOfQmlimportscanner>"), depencenciesData,
                             dependencies, QStringList(pluginImportUri))) {
        std::cerr << "failed to proecess output of qmlimportscanner" << std::endl;
        return false;
    }
    return true;
}

bool compactDependencies(QStringList *dependencies)
{
    if (dependencies->isEmpty())
        return false;
    dependencies->sort();
    QStringList oldDep = dependencies->first().split(QLatin1Char(' '));
    Q_ASSERT(oldDep.size() == 2);
    int oldPos = 0;
    for (int idep = 1; idep < dependencies->size(); ++idep) {
        QString depStr = dependencies->at(idep);
        const QStringList newDep = depStr.split(QLatin1Char(' '));
        Q_ASSERT(newDep.size() == 2);
        if (newDep.first() != oldDep.first()) {
            if (++oldPos != idep)
                dependencies->replace(oldPos, depStr);
            oldDep = newDep;
        } else {
            QStringList v1 = oldDep.last().split(QLatin1Char('.'));
            QStringList v2 = newDep.last().split(QLatin1Char('.'));
            Q_ASSERT(v1.size() == 2);
            Q_ASSERT(v2.size() == 2);
            bool ok;
            int major1 = v1.first().toInt(&ok);
            Q_ASSERT(ok);
            int major2 = v2.first().toInt(&ok);
            Q_ASSERT(ok);
            if (major1 != major2) {
                std::cerr << "Found a dependency on " << qPrintable(oldDep.first())
                          << " with two major versions:" << qPrintable(oldDep.last())
                          << " and " << qPrintable(newDep.last())
                          << " which is unsupported, discarding smaller version" << std::endl;
                if (major1 < major2)
                    dependencies->replace(oldPos, depStr);
            } else {
                int minor1 = v1.last().toInt(&ok);
                Q_ASSERT(ok);
                int minor2 = v2.last().toInt(&ok);
                Q_ASSERT(ok);
                if (minor1 < minor2)
                    dependencies->replace(oldPos, depStr);
            }
        }
    }
    if (++oldPos < dependencies->size()) {
        *dependencies = dependencies->mid(0, oldPos);
        return true;
    }
    return false;
}

inline std::wostream &operator<<(std::wostream &str, const QString &s)
{
#ifdef Q_OS_WIN
    str << reinterpret_cast<const wchar_t *>(s.utf16());
#else
    str << s.toStdWString();
#endif
    return str;
}

void printDebugMessage(QtMsgType, const QMessageLogContext &, const QString &msg)
{
    std::wcerr << msg << std::endl;
    // In case of QtFatalMsg the calling code will abort() when appropriate.
}


int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN) && !defined(Q_CC_MINGW)
    // we do not want windows popping up if the module loaded triggers an assert
    SetErrorMode(SEM_NOGPFAULTERRORBOX);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif // Q_OS_WIN && !Q_CC_MINGW
    // The default message handler might not print to console on some systems. Enforce this.
    qInstallMessageHandler(printDebugMessage);
#ifdef Q_OS_UNIX
    // qmldump may crash, but we don't want any crash handlers to pop up
    // therefore we intercept the segfault and just exit() ourselves
    struct sigaction sigAction;

    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_handler = &sigSegvHandler;
    sigAction.sa_flags   = 0;

    sigaction(SIGSEGV, &sigAction, 0);
#endif

#ifdef QT_SIMULATOR
    // Running this application would bring up the Qt Simulator (since it links Qt GUI), avoid that!
    QtSimulatorPrivate::SimulatorConnection::createStubInstance();
#endif

    // don't require a window manager even though we're a QGuiApplication
    bool requireWindowManager = false;
    for (int index = 1; index < argc; ++index) {
        if (QString::fromLocal8Bit(argv[index]) == "--defaultplatform"
                || QString::fromLocal8Bit(argv[index]) == "-defaultplatform") {
            requireWindowManager = true;
            break;
        }
    }

    if (!requireWindowManager)
        qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("minimal"));

    QGuiApplication app(argc, argv);
    const QStringList args = app.arguments();
    const QString appName = QFileInfo(app.applicationFilePath()).baseName();
    if (args.size() < 2) {
        printUsage(appName);
        return EXIT_INVALIDARGUMENTS;
    }

    QString pluginImportUri;
    QString pluginImportVersion;
    bool relocatable = true;
    QString dependenciesFile;
    enum Action { Uri, Path, Builtins };
    Action action = Uri;
    {
        QStringList positionalArgs;

        for (int iArg = 0; iArg < args.size(); ++iArg) {
            const QString &arg = args.at(iArg);
            if (!arg.startsWith(QLatin1Char('-'))) {
                positionalArgs.append(arg);
                continue;
            }
            if (arg == QLatin1String("--dependencies")
                    || arg == QLatin1String("-dependencies")) {
                if (++iArg == args.size()) {
                    std::cerr << "missing dependencies file" << std::endl;
                    return EXIT_INVALIDARGUMENTS;
                }
                dependenciesFile = args.at(iArg);
            } else if (arg == QLatin1String("--notrelocatable")
                    || arg == QLatin1String("-notrelocatable")
                    || arg == QLatin1String("--nonrelocatable")
                    || arg == QLatin1String("-nonrelocatable")) {
                relocatable = false;
            } else if (arg == QLatin1String("--relocatable")
                        || arg == QLatin1String("-relocatable")) {
                relocatable = true;
            } else if (arg == QLatin1String("--noinstantiate")
                       || arg == QLatin1String("-noinstantiate")) {
                creatable = false;
            } else if (arg == QLatin1String("--path")
                       || arg == QLatin1String("-path")) {
                action = Path;
            } else if (arg == QLatin1String("--builtins")
                       || arg == QLatin1String("-builtins")) {
                action = Builtins;
            } else if (arg == QLatin1String("-v")) {
                verbose = true;
            } else if (arg == QLatin1String("--defaultplatform")
                       || arg == QLatin1String("-defaultplatform")) {
                continue;
            } else {
                std::cerr << "Invalid argument: " << qPrintable(arg) << std::endl;
                return EXIT_INVALIDARGUMENTS;
            }
        }

        if (action == Uri) {
            if (positionalArgs.size() != 3 && positionalArgs.size() != 4) {
                std::cerr << "Incorrect number of positional arguments" << std::endl;
                return EXIT_INVALIDARGUMENTS;
            }
            pluginImportUri = positionalArgs[1];
            pluginImportVersion = positionalArgs[2];
            if (positionalArgs.size() >= 4)
                pluginImportPath = positionalArgs[3];
        } else if (action == Path) {
            if (positionalArgs.size() != 2 && positionalArgs.size() != 3) {
                std::cerr << "Incorrect number of positional arguments" << std::endl;
                return EXIT_INVALIDARGUMENTS;
            }
            pluginImportPath = QDir::fromNativeSeparators(positionalArgs[1]);
            if (positionalArgs.size() == 3)
                pluginImportVersion = positionalArgs[2];
        } else if (action == Builtins) {
            if (positionalArgs.size() != 1) {
                std::cerr << "Incorrect number of positional arguments" << std::endl;
                return EXIT_INVALIDARGUMENTS;
            }
        }
    }

    QQmlEngine engine;
    if (!pluginImportPath.isEmpty()) {
        QDir cur = QDir::current();
        cur.cd(pluginImportPath);
        pluginImportPath = cur.canonicalPath();
        QDir::setCurrent(pluginImportPath);
        engine.addImportPath(pluginImportPath);
    }
    bool calculateDependencies = !pluginImportUri.isEmpty() && !pluginImportVersion.isEmpty();
    QStringList dependencies;
    if (!dependenciesFile.isEmpty())
        calculateDependencies = !readDependenciesFile(dependenciesFile, &dependencies,
                                                      QStringList(pluginImportUri)) && calculateDependencies;
    if (calculateDependencies)
        getDependencies(engine, pluginImportUri, pluginImportVersion, &dependencies);
    compactDependencies(&dependencies);

    // load the QtQml 2.2 builtins and the dependencies
    {
        QByteArray code("import QtQml 2.2");
        foreach (const QString &moduleToImport, dependencies) {
            code.append("\nimport ");
            code.append(moduleToImport.toUtf8());
        }
        code.append("\nQtObject {}");
        QQmlComponent c(&engine);
        c.setData(code, QUrl::fromLocalFile(pluginImportPath + "/loaddependencies.qml"));
        c.create();
        if (!c.errors().isEmpty()) {
            foreach (const QQmlError &error, c.errors())
                std::cerr << qPrintable( error.toString() ) << std::endl;
            return EXIT_IMPORTERROR;
        }
    }

    // find all QMetaObjects reachable from the builtin module
    QSet<const QMetaObject *> uncreatableMetas;
    QSet<const QMetaObject *> singletonMetas;
    QSet<const QMetaObject *> defaultReachable = collectReachableMetaObjects(&engine, uncreatableMetas, singletonMetas);
    QList<QQmlType *> defaultTypes = QQmlMetaType::qmlTypes();

    // add some otherwise unreachable QMetaObjects
    defaultReachable.insert(&QQuickMouseEvent::staticMetaObject);
    // QQuickKeyEvent, QQuickPinchEvent, QQuickDropEvent are not exported
    QSet<QByteArray> defaultReachableNames;

    // this will hold the meta objects we want to dump information of
    QSet<const QMetaObject *> metas;

    if (action == Builtins) {
        metas = defaultReachable;
    } else {
        // find a valid QtQuick import
        QByteArray importCode;
        QQmlType *qtObjectType = QQmlMetaType::qmlType(&QObject::staticMetaObject);
        if (!qtObjectType) {
            std::cerr << "Could not find QtObject type" << std::endl;
            importCode = QByteArray("import QtQml 2.2");
        } else {
            QString module = qtObjectType->qmlTypeName();
            module = module.mid(0, module.lastIndexOf(QLatin1Char('/')));
            importCode = QString("import %1 %2.%3").arg(module,
                                                        QString::number(qtObjectType->majorVersion()),
                                                        QString::number(qtObjectType->minorVersion())).toUtf8();
        }
        // avoid importing dependencies?
        foreach (const QString &moduleToImport, dependencies) {
            importCode.append("\nimport ");
            importCode.append(moduleToImport.toUtf8());
        }

        // find all QMetaObjects reachable when the specified module is imported
        if (action != Path) {
            importCode += QString("\nimport %0 %1\n").arg(pluginImportUri, pluginImportVersion).toLatin1();
        } else {
            // pluginImportVersion can be empty
            importCode += QString("\nimport \".\" %2\n").arg(pluginImportVersion).toLatin1();
        }

        // create a component with these imports to make sure the imports are valid
        // and to populate the declarative meta type system
        {
            QByteArray code = importCode;
            code += "\nQtObject {}";
            QQmlComponent c(&engine);

            c.setData(code, QUrl::fromLocalFile(pluginImportPath + "/typelist.qml"));
            c.create();
            if (!c.errors().isEmpty()) {
                foreach (const QQmlError &error, c.errors())
                    std::cerr << qPrintable( error.toString() ) << std::endl;
                return EXIT_IMPORTERROR;
            }
        }

        QSet<const QMetaObject *> candidates = collectReachableMetaObjects(&engine, uncreatableMetas, singletonMetas, defaultTypes);
        candidates.subtract(defaultReachable);

        // Also eliminate meta objects with the same classname.
        // This is required because extended objects seem not to share
        // a single meta object instance.
        foreach (const QMetaObject *mo, defaultReachable)
            defaultReachableNames.insert(QByteArray(mo->className()));
        foreach (const QMetaObject *mo, candidates) {
            if (!defaultReachableNames.contains(mo->className()))
                metas.insert(mo);
        }
    }

    // setup static rewrites of type names
    cppToId.insert("QString", "string");
    cppToId.insert("QQmlEasingValueType::Type", "Type");

    // start dumping data
    QByteArray bytes;
    QmlStreamWriter qml(&bytes);

    qml.writeStartDocument();
    qml.writeLibraryImport(QLatin1String("QtQuick.tooling"), 1, 2);
    qml.write(QString("\n"
              "// This file describes the plugin-supplied types contained in the library.\n"
              "// It is used for QML tooling purposes only.\n"
              "//\n"
              "// This file was auto-generated by:\n"
              "// '%1 %2'\n"
              "\n").arg(QFileInfo(args.at(0)).fileName()).arg(QStringList(args.mid(1)).join(QLatin1String(" "))));
    qml.writeStartObject("Module");
    QStringList quotedDependencies;
    foreach (const QString &dep, dependencies)
        quotedDependencies << enquote(dep);
    qml.writeArrayBinding("dependencies", quotedDependencies);

    // put the metaobjects into a map so they are always dumped in the same order
    QMap<QString, const QMetaObject *> nameToMeta;
    foreach (const QMetaObject *meta, metas)
        nameToMeta.insert(convertToId(meta), meta);

    Dumper dumper(&qml);
    if (relocatable)
        dumper.setRelocatableModuleUri(pluginImportUri);
    foreach (const QMetaObject *meta, nameToMeta) {
        dumper.dump(meta, uncreatableMetas.contains(meta), singletonMetas.contains(meta));
    }
    foreach (const QQmlType *compositeType, qmlTypesByCompositeName)
        dumper.dumpComposite(&engine, compositeType, defaultReachableNames);

    // define QEasingCurve as an extension of QQmlEasingValueType, this way
    // properties using the QEasingCurve type get useful type information.
    if (pluginImportUri.isEmpty())
        dumper.writeEasingCurve();

    qml.writeEndObject();
    qml.writeEndDocument();

    std::cout << bytes.constData() << std::flush;

    // workaround to avoid crashes on exit
    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(0);
    QObject::connect(&timer, SIGNAL(timeout()), &app, SLOT(quit()));
    timer.start();

    return app.exec();
}
