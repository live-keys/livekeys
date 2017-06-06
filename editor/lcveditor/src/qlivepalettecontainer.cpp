/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#include "qlivepalettecontainer.h"
#include <QQmlEngine>
#include <QQmlComponent>
#include <QDir>
#include <QDirIterator>

#include "qcodeconverter.h"
#include "qlivepalette.h"

#include <QDebug>

//#define QPALETTE_CONTAINER_DEBUG_FLAG
#ifdef QPALETTE_CONTAINER_DEBUG_FLAG
#define QPALETTE_CONTAINER_DEBUG(_param) qDebug() << "PALETTES:" << (_param)
#else
#define QPALETTE_CONTAINER_DEBUG(_param)
#endif

namespace lcv{

class QLivePaletteLoader{
public:
    QLivePaletteLoader(const QString& path, const QString& type, const QString& typeObject = "")
        : m_converter(0)
        , m_path(path)
        , m_type(type)
        , m_typeObject(typeObject)
    {}

    QCodeConverter* getItem(QQmlEngine* engine);

private:
    void handleError(const QQmlComponent &component) const;

    QCodeConverter* m_converter;

    QString m_path;
    QString m_type;
    QString m_typeObject;
};

QCodeConverter *QLivePaletteLoader::getItem(QQmlEngine *engine){
    if ( m_converter )
        return m_converter;

    QPALETTE_CONTAINER_DEBUG("Loading palette: " + m_path);

    QQmlComponent component(engine, QUrl::fromLocalFile(m_path), QQmlComponent::PreferSynchronous);
    if ( component.isError() || component.isNull() ){
        qCritical("Failed to load palette: %s", qPrintable(m_path));
        handleError(component);
        return 0;
    }

    QObject *obj = component.create();
    if ( obj == 0 || component.isError() ){
        qCritical("Failed to create object from palette: %s", qPrintable(m_path));
        handleError(component);
        return 0;
    }

    m_converter = static_cast<lcv::QCodeConverter*>(obj);
    if ( !m_converter ){
        qCritical("Failed to load palette: \'%s\'. Value is not a LivePalette or CodeConverter type.",
                  qPrintable(m_path));
        return 0;
    }

    if ( m_converter->type().isEmpty() || m_converter->serialize() == 0 ){
        qCritical("Failed to load palette: \'%s\'. Palette/CodeConverter must have a type and serializer defined",
                  qPrintable(m_path));
        return 0;
    }

    //TODO: Check converter type with m_type, and also check typeobject

    QPALETTE_CONTAINER_DEBUG("Loaded palette on type: \'" + m_converter->type() + "\' " + m_converter->typeObject());

    return m_converter;
}

void QLivePaletteLoader::handleError(const QQmlComponent &component) const{
    foreach ( const QQmlError& error, component.errors() ){
        qWarning(
            "\'%s\':%d,%d %s",
            qPrintable(error.url().toString()),
            error.line(),
            error.column(),
            qPrintable(error.description())
        );
    }
}


// QLivePaletteContainerPrivate
// ----------------------------

class QLivePaletteContainerPrivate{
public:
    typedef QHash<QString, QHash<QString, QLivePaletteLoader*> > PaletteHash;

    QQmlEngine* engine;
    PaletteHash items;
};


// QLivePaletteContainer
// ---------------------

QLivePaletteContainer::QLivePaletteContainer(QQmlEngine *engine)
    : d_ptr(new QLivePaletteContainerPrivate)
{
    Q_D(QLivePaletteContainer);
    d->engine = engine;
}

QLivePaletteContainer::~QLivePaletteContainer(){
    delete d_ptr;
}

void QLivePaletteContainer::scanPalettes(const QString &path){
    QDirIterator dit(path);
    while ( dit.hasNext() ){
        dit.next();
        QFileInfo info = dit.fileInfo();
        if ( info.fileName() == "." || info.fileName() == ".." )
            continue;

        if ( info.isDir() ){
            scanPalettes(info.filePath());
        } else if ( dit.fileName() == "palettedir" ){
            scanPaletteDir(path);
        }
    }
}

void QLivePaletteContainer::scanPaletteDir(const QString &path){
    Q_D(QLivePaletteContainer);

    QString paletteDirPath = QDir::cleanPath(path + "/" + "palettedir");

    QPALETTE_CONTAINER_DEBUG("Scanning palettedir: " + paletteDirPath);

    QFile f(paletteDirPath);
    if ( !f.open(QIODevice::ReadOnly) ){
        qWarning("Failed to read palettedir file: %s", qPrintable(paletteDirPath));
        return;
    }

    int lineNumber = 0;
    while ( !f.atEnd() ){
        QByteArray line = f.readLine().trimmed();
        if ( !line.isEmpty() && !line.startsWith("#") ){
            QList<QByteArray> segments = line.split(' ');
            if ( segments.size() == 2 ){
                QString palettePath = QDir::cleanPath(path + "/" + segments[1].trimmed());
                QString type = segments[0].trimmed();
                QPALETTE_CONTAINER_DEBUG("Adding palette: \'" + palettePath + "\' on \'" + type + "\'");
                d->items[type].insert("", new QLivePaletteLoader(palettePath, type));
            } else if ( segments.size() == 3 ){
                QString palettePath = QDir::cleanPath(path + "/" + segments[2].trimmed());
                QString type = segments[0].trimmed();
                QString typeObject = segments[1].trimmed();
                QPALETTE_CONTAINER_DEBUG(
                    "Adding palette: \'" + palettePath + "\' on \'" + type + "\' for object \'" +
                    typeObject + "\'"
                );
                d->items[type].insert(
                    typeObject, new QLivePaletteLoader(palettePath, type, typeObject)
                );
            } else {
                qCritical("Failed to parse line %d in \'%s\'. Palettes require <type> [<type-object>] <file>",
                          lineNumber, qPrintable(paletteDirPath));
            }
        }
        ++lineNumber;
    }
}

QLivePaletteContainer *QLivePaletteContainer::create(QQmlEngine *engine, const QString &path){
    QLivePaletteContainer* ct = new QLivePaletteContainer(engine);
    ct->scanPalettes(path);
    return ct;
}

QCodeConverter *QLivePaletteContainer::findPalette(const QString &type, const QString &object){
    Q_D(QLivePaletteContainer);
    QLivePaletteContainerPrivate::PaletteHash::Iterator it = d->items.find(type);
    if ( it == d->items.end() )
        return 0;

    // lookup specific palette
    QHash<QString, QLivePaletteLoader*>::iterator typeit = it.value().find(object);
    if ( typeit == it.value().end() ){

        // lookup general palette
        typeit = it->find("");
        if ( typeit == it->end() )
            return 0;
        return typeit.value()->getItem(d->engine);
    }
    return typeit.value()->getItem(d->engine);
}

QCodeConverter *QLivePaletteContainer::findPalette(const QString &type, const QStringList &object){
    return findPalette(type, object.isEmpty() ? "" : type);
}

int QLivePaletteContainer::size() const{
    Q_D(const QLivePaletteContainer);
    return d->items.size();
}

}// namespace

