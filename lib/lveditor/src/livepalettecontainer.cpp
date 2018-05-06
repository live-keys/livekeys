/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#include "live/livepalettecontainer.h"
#include "live/codeconverter.h"
#include "live/livepalette.h"
#include "live/visuallog.h"
#include "live/livepalettelist.h"

#include <QQmlEngine>
#include <QQmlComponent>
#include <QDir>
#include <QDirIterator>
#include <QDebug>

namespace lv{

class LivePaletteLoader{
public:
    LivePaletteLoader(const QString& path, const QString& type, const QString& typeObject = "")
        : m_factory(0)
        , m_path(path)
        , m_type(type)
        , m_typeObject(typeObject)
    {}
    ~LivePaletteLoader(){
        delete m_factory;
    }

    CodeConverter* getItem(QQmlEngine* engine);
    const QString& path() const{ return m_path; }

private:
    void handleError(const QQmlComponent &component) const;

    QQmlComponent* m_factory;

    QString m_path;
    QString m_type;
    QString m_typeObject;
};

CodeConverter *LivePaletteLoader::getItem(QQmlEngine *engine){
    if ( !m_factory ){
        vlog_debug("editor-livepaletteloader", "Loading palette: " + m_path);

        m_factory = new QQmlComponent(engine, QUrl::fromLocalFile(m_path), QQmlComponent::PreferSynchronous);
        if ( m_factory->isError() || m_factory->isNull() ){
            qCritical("Failed to load palette: %s", qPrintable(m_path));
            handleError(*m_factory);
            return 0;
        }
    }

    QObject *obj = m_factory->create();
    if ( obj == 0 || m_factory->isError() ){
        qCritical("Failed to create object from palette: %s", qPrintable(m_path));
        handleError(*m_factory);
        return 0;
    }

    CodeConverter* converter = static_cast<lv::CodeConverter*>(obj);
    if ( !converter ){
        qCritical("Failed to load palette: \'%s\'. Value is not a LivePalette or CodeConverter type.",
                  qPrintable(m_path));
        return 0;
    }

    if ( converter->type().isEmpty() || converter->serialize() == 0 ){
        qCritical("Failed to load palette: \'%s\'. Palette/CodeConverter must have a type and serializer defined",
                  qPrintable(m_path));
        return 0;
    }

    //TODO: Check converter type with m_type, and also check typeobject

    vlog_debug("editor-livepaletteloader", "Loaded palette on type: \'" + m_converter->type() + "\' " + m_converter->typeObject());

    return converter;
}

void LivePaletteLoader::handleError(const QQmlComponent &component) const{
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

class LivePaletteContainerPrivate{
public:
    typedef QHash<QString, QMultiHash<QString, LivePaletteLoader*> > PaletteHash;

    QQmlEngine* engine;
    PaletteHash items;
};


// QLivePaletteContainer
// ---------------------

LivePaletteContainer::LivePaletteContainer(QQmlEngine *engine)
    : d_ptr(new LivePaletteContainerPrivate)
{
    Q_D(LivePaletteContainer);
    d->engine = engine;
}

LivePaletteContainer::~LivePaletteContainer(){
    delete d_ptr;
}

void LivePaletteContainer::scanPalettes(const QString &path){
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

void LivePaletteContainer::scanPaletteDir(const QString &path){
    Q_D(LivePaletteContainer);

    QString paletteDirPath = QDir::cleanPath(path + "/" + "palettedir");

    vlog_debug("editor-livepaletteloader", "Scanning palettedir: " + paletteDirPath);

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
                vlog_debug("editor-livepaletteloader", "Adding palette: \'" + palettePath + "\' on \'" + type + "\'");
                d->items[type].insert("", new LivePaletteLoader(palettePath, type));
            } else if ( segments.size() == 3 ){
                QString palettePath = QDir::cleanPath(path + "/" + segments[2].trimmed());
                QString type = segments[0].trimmed();
                QString typeObject = segments[1].trimmed();
                vlog_debug("editor-livepaletteloader",
                    "Adding palette: \'" + palettePath + "\' on \'" + type + "\' for object \'" +
                    typeObject + "\'"
                );
                d->items[type].insert(
                    typeObject, new LivePaletteLoader(palettePath, type, typeObject)
                );
            } else {
                qCritical("Failed to parse line %d in \'%s\'. Palettes require <type> [<type-object>] <file>",
                          lineNumber, qPrintable(paletteDirPath));
            }
        }
        ++lineNumber;
    }
}

LivePaletteContainer *LivePaletteContainer::create(QQmlEngine *engine, const QString &path){
    LivePaletteContainer* ct = new LivePaletteContainer(engine);
    ct->scanPalettes(path);
    return ct;
}

CodeConverter *LivePaletteContainer::findPalette(const QString &type, const QString &object){
    Q_D(LivePaletteContainer);
    LivePaletteContainerPrivate::PaletteHash::Iterator it = d->items.find(type);
    if ( it == d->items.end() )
        return 0;

    // lookup specific palette
    QHash<QString, LivePaletteLoader*>::iterator typeit = it.value().find(object);
    if ( typeit == it.value().end() ){

        // lookup general palette
        typeit = it->find("");
        if ( typeit == it->end() )
            return 0;
        return typeit.value()->getItem(d->engine);
    }
    return typeit.value()->getItem(d->engine);
}

CodeConverter *LivePaletteContainer::findPalette(const QString &type, const QStringList &object){
    return findPalette(type, object.isEmpty() ? "" : type);
}

LivePaletteList* LivePaletteContainer::findPalettes(const QString &type, const QString &object){
    Q_D(LivePaletteContainer);
    LivePaletteList* l = new LivePaletteList(this);
    d->engine->setObjectOwnership(l, QQmlEngine::JavaScriptOwnership);

    LivePaletteContainerPrivate::PaletteHash::Iterator it = d->items.find(type);
    if ( it == d->items.end() )
        return l;

    QHash<QString, LivePaletteLoader*>::iterator typeIt = it.value().begin();
    while ( typeIt != it.value().end() ){
        qDebug() << typeIt.value()->path() << type;
        if ( typeIt.key() == "" || typeIt.key() == object )
            l->append(typeIt.value());
        ++typeIt;
    }

    return l;
}

LivePaletteList *LivePaletteContainer::findPalettes(const QString &type, const QStringList &object){
    return findPalettes(type, object.isEmpty() ? "" : object.last());
}

QString LivePaletteContainer::paletteName(LivePaletteLoader *loader){
    return QFileInfo(loader->path()).baseName();
}

LivePalette *LivePaletteContainer::createPalette(LivePaletteLoader *loader){
    Q_D(LivePaletteContainer);
    CodeConverter* cvt = loader->getItem(d->engine);
    LivePalette* lp = qobject_cast<LivePalette*>(cvt);
    if ( !lp ){
        delete cvt;
        return 0;
    }

    lp->setPath(loader->path());

    d->engine->setObjectOwnership(lp, QQmlEngine::JavaScriptOwnership);
    return lp;
}

int LivePaletteContainer::size() const{
    Q_D(const LivePaletteContainer);
    return d->items.size();
}

}// namespace

