/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
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

#include "live/palettecontainer.h"
#include "live/codepalette.h"
#include "live/visuallog.h"
#include "live/visuallogqt.h"
#include "live/palettelist.h"

#include <QQmlEngine>
#include <QQmlComponent>
#include <QDir>
#include <QDirIterator>
#include <QDebug>

/**
 * \class lv::PaletteContainer
 * \brief Stores all the palettes that are available in LiveKeys.
 *
 * More palettes can get populated when a new plugin is added.
 * \ingroup lveditor
 */
namespace lv{

/// \private
class PaletteLoader{
public:
    PaletteLoader(const QString& path, const QString& type)
        : m_factory(0)
        , m_path(path)
        , m_type(type)
    {}
    ~PaletteLoader(){
        delete m_factory;
    }

    CodePalette* getItem(QQmlEngine* engine);
    const QString& path() const{ return m_path; }

private:
    void handleError(const QQmlComponent &component) const;

    QQmlComponent* m_factory;

    QString m_path;
    QString m_type;
};

CodePalette *PaletteLoader::getItem(QQmlEngine *engine){
    if ( !m_factory ){
        vlog_debug("editor-codepaletteloader", "Loading palette: " + m_path);

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

    CodePalette* palette = static_cast<lv::CodePalette*>(obj);
    if ( !palette ){
        qCritical("Failed to load palette: \'%s\'. Value is not a CodePalette type.",
                  qPrintable(m_path));
        return 0;
    }

    if ( palette->type().isEmpty() ){
        qCritical("Failed to load palette: \'%s\'. Palette must have a type defined",
                  qPrintable(m_path));
        return 0;
    }

    vlog_debug("editor-codepaletteloader", "Loaded palette on type: \'" + m_converter->type() + "\' " + m_converter->typeObject());

    return palette;
}

void PaletteLoader::handleError(const QQmlComponent &component) const{
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

/// \private
class PaletteContainerPrivate{
public:
    typedef QMultiHash<QString, PaletteLoader*> PaletteHash;

    QQmlEngine* engine;
    PaletteHash items;
};


// QLivePaletteContainer
// ---------------------

PaletteContainer::PaletteContainer(QQmlEngine *engine)
    : d_ptr(new PaletteContainerPrivate)
{
    Q_D(PaletteContainer);
    d->engine = engine;
}

PaletteContainer::~PaletteContainer(){
    delete d_ptr;
}

/**
 * \brief Recursively scans the given folder path for plugins
 */
void PaletteContainer::scanPalettes(const QString &path){
    QDirIterator dit(path);
    while ( dit.hasNext() ){
        dit.next();
        QFileInfo info = dit.fileInfo();
        if ( info.fileName() == "." || info.fileName() == ".." )
            continue;

        if ( info.isDir() ){
            if ( Plugin::existsIn(info.filePath().toStdString() ) )
                scanPalettes(Plugin::createFromPath(info.filePath().toStdString()));

            scanPalettes(info.filePath());
        }
    }
}

/**
 * \brief Extract the palettes from the given plugin
 */
void PaletteContainer::scanPalettes(Plugin::Ptr plugin){
    Q_D(PaletteContainer);

    vlog("editor-codepaletteloader").v() << "Scanning plugin: " << plugin->name();

    for ( auto it = plugin->palettes().begin(); it != plugin->palettes().end(); ++it ){
        QString palettePath = QDir::cleanPath(QString::fromStdString(plugin->path() + "/" + it->first));
        QString type = QString::fromStdString(it->second);

        vlog("editor-codepaletteloader").v() << "Adding palette by type: \'" + palettePath + "\' on \'" + type + "\'";

        d->items.insert(type, new PaletteLoader(palettePath, type));
    }
}

/**
 * \brief PalleteContainer creator than simultaneously scans the given path and loads them
 */
PaletteContainer *PaletteContainer::create(QQmlEngine *engine, const QString &path){
    PaletteContainer* ct = new PaletteContainer(engine);
    ct->scanPalettes(path);
    return ct;
}

/**
 * \brief Finds a palette with a specific type
 */
PaletteLoader *PaletteContainer::findPalette(const QString &type) const{
    Q_D(const PaletteContainer);

    PaletteContainerPrivate::PaletteHash::ConstIterator it = d->items.find(type);
    if ( it == d->items.end() )
        return nullptr;
    return it.value();
}

/**
 * \brief Find a list of palettes of a given type
 *
 * This list is Javascript-owned!
 */
PaletteList *PaletteContainer::findPalettes(const QString &type, lv::PaletteList *l){
    Q_D(PaletteContainer);

    if ( !l ){
        l = new PaletteList(this);
        d->engine->setObjectOwnership(l, QQmlEngine::JavaScriptOwnership);
    }

    PaletteContainerPrivate::PaletteHash::Iterator it = d->items.find(type);

    while ( it != d->items.end() && it.key() == type ){
        l->append(it.value());
        ++it;
    }

    return l;
}

/**
 * \brief Find the number of palettes of a given type
 */
int PaletteContainer::countPalettes(const QString &type) const{
    Q_D(const PaletteContainer);

    int result = 0;

    PaletteContainerPrivate::PaletteHash::ConstIterator it = d->items.find(type);
    while ( it != d->items.end() && it.key() == type ){
        ++result;
        ++it;
    }

    return result;
}

/**
 * \brief Get palette name from the interal PaletteLoader object
 */
QString PaletteContainer::paletteName(PaletteLoader *loader){
    return QFileInfo(loader->path()).baseName();
}

/**
 * \brief Get palette path from the internal PaletteLoader object
 */
const QString &PaletteContainer::palettePath(PaletteLoader *loader){
    return loader->path();
}

/**
 * \brief Instantiates a new palette from the PaletteLoader
 *
 * Again, this object has Javascript ownership
 */
CodePalette *PaletteContainer::createPalette(PaletteLoader *loader){
    Q_D(PaletteContainer);
    CodePalette* cp = loader->getItem(d->engine);
    if ( !cp )
        return nullptr;

    cp->setPath(loader->path());
    d->engine->setObjectOwnership(cp, QQmlEngine::JavaScriptOwnership);
    return cp;
}

/**
 * \brief Return number of palette inside the container
 */
int PaletteContainer::size() const{
    Q_D(const PaletteContainer);
    return d->items.size();
}

}// namespace

