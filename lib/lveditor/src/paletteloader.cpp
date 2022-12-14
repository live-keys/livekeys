/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#include "paletteloader.h"
#include "live/codepalette.h"
#include "live/project.h"
#include "live/visuallog.h"
#include "live/visuallogqt.h"
#include "live/viewengine.h"

#include "live/mlnode.h"
#include "live/mlnodetojson.h"
#include "live/mlnodetoqml.h"
#include "live/packagegraph.h"
#include "live/palettecontainer.h"

#include <QQmlEngine>
#include <QQmlContext>
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

// PaletteComponent
// ----------------------------------------------------------------------------

/// \private
class PaletteComponent{
public:
    PaletteComponent(const QString& path, const QString& type);
    ~PaletteComponent();

    CodePalette* getItem(QQmlEngine* engine);
    QJSValue getContent(ViewEngine *engine);
    const QString& path() const{ return m_path; }
    const QString& type() const{ return m_type; }
    const QString& name() const{ return m_name; }
    bool configuresLayout() const{ return m_configuresLayout; }
private:
    void handleError(const QQmlComponent &component) const;

    QQmlComponent* m_factory;

    QString m_name;
    QString m_path;
    QString m_type;
    bool    m_configuresLayout;
};

PaletteComponent::PaletteComponent(const QString &path, const QString &type)
    : m_factory(nullptr)
    , m_path(path)
    , m_type(type)
{
    m_name = QFileInfo(m_path).baseName();
    m_configuresLayout = m_path.endsWith(".json");
}

PaletteComponent::~PaletteComponent(){
    delete m_factory;
}

CodePalette *PaletteComponent::getItem(QQmlEngine *engine){
    if ( !m_factory ){
        vlog_debug("editor-codePaletteComponent", "Loading palette: " + m_path);

        m_factory = new QQmlComponent(engine, QUrl::fromLocalFile(m_path), QQmlComponent::PreferSynchronous);
        if ( m_factory->isError() || m_factory->isNull() ){
            qCritical("Failed to load palette: %s", qPrintable(m_path));
            handleError(*m_factory);
            return nullptr;
        }
    }

    QObject *obj = m_factory->create();
    if ( obj == nullptr || m_factory->isError() ){
        qCritical("Failed to create object from palette: %s", qPrintable(m_path));
        handleError(*m_factory);
        return nullptr;
    }

    CodePalette* palette = static_cast<lv::CodePalette*>(obj);
    if ( !palette ){
        qCritical("Failed to load palette: \'%s\'. Value is not a CodePalette type.",
                  qPrintable(m_path));
        return nullptr;
    }

    if ( palette->type().isEmpty() ){
        qCritical("Failed to load palette: \'%s\'. Palette must have a type defined",
                  qPrintable(m_path));
        return nullptr;
    }

    vlog_debug("editor-codePaletteComponent", "Loaded palette on type: \'" + m_converter->type() + "\' " + m_converter->typeObject());

    return palette;
}

QJSValue PaletteComponent::getContent(ViewEngine *viewEngine){
    if ( !m_configuresLayout )
        return QJSValue();
    Project* pr = Project::grabFromLayer(viewEngine);
    if ( !pr )
        return QJSValue();

    std::string content = viewEngine->fileIO()->readFromFile(m_path.toStdString());

    try {
        MLNode result;
        QJSValue jsResult;
        ml::fromJson(content, result);
        ml::toQml(result, jsResult, viewEngine->engine());
        return jsResult;
    } catch (Exception& e) {
        qWarning("Failed to parse '%s': %s", qPrintable(m_path), e.message().c_str());
    }
    return QJSValue();
}


void PaletteComponent::handleError(const QQmlComponent &component) const{
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


// LivePaletteContainerPrivate
// ----------------------------------------------------------------------------

/// \private
class PaletteLoaderPrivate{
public:
    typedef QHash<QString, PaletteComponent*> PaletteHash;

    ViewEngine* engine;
    PaletteHash componentCache;

    PaletteComponent* getCacheComponent(const PaletteContainer::PaletteInfo& pi);
};

PaletteComponent *PaletteLoaderPrivate::getCacheComponent(const PaletteContainer::PaletteInfo &pi){
    QString palettePath = QString::fromStdString(pi.path().data());
    QString paletteType = QString::fromStdString(pi.type().data());

    PaletteComponent* paletteComponent = nullptr;

    auto it = componentCache.find(palettePath);
    if ( it != componentCache.end() ){
        paletteComponent = it.value();
    } else {
        paletteComponent = new PaletteComponent(palettePath, paletteType);
        componentCache.insert(palettePath, paletteComponent);
    }

    return paletteComponent;
}


// LivePaletteContainer
// ----------------------------------------------------------------------------

PaletteLoader::PaletteLoader(ViewEngine *engine)
    : d_ptr(new PaletteLoaderPrivate)
{
    Q_D(PaletteLoader);
    d->engine = engine;
}

PaletteLoader::~PaletteLoader(){
    delete d_ptr;
}

/**
 * \brief PalleteContainer creator than simultaneously scans the given path and loads them
 */
PaletteLoader *PaletteLoader::create(ViewEngine *engine){
    PaletteLoader* ct = new PaletteLoader(engine);
    return ct;
}

PaletteContainer::PaletteInfo PaletteLoader::findPaletteByName(const QString &name) const{
    Q_D(const PaletteLoader);
    auto pc = d->engine->packageGraph() ? d->engine->packageGraph()->paletteContainer() : nullptr;
    if (!pc)
        return PaletteContainer::PaletteInfo();

    return pc->findPaletteByName(name.toStdString());
}

PaletteContainer::PaletteInfo PaletteLoader::findFirstPalette(const QString &type) const{
    Q_D(const PaletteLoader);
    auto pc = d->engine->packageGraph() ? d->engine->packageGraph()->paletteContainer() : nullptr;
    if (!pc)
        return PaletteContainer::PaletteInfo();

    return pc->findFirstPalette(type.toStdString());
}

std::list<PaletteContainer::PaletteInfo> PaletteLoader::findPalettes(const QString &type) const{
    Q_D(const PaletteLoader);
    auto pc = d->engine->packageGraph() ? d->engine->packageGraph()->paletteContainer() : nullptr;
    if (!pc)
        return std::list<PaletteContainer::PaletteInfo>();

    return pc->findPalettes(type.toStdString());
}

CodePalette *PaletteLoader::createPalette(const PaletteContainer::PaletteInfo &pi){
    Q_D(PaletteLoader);
    PaletteComponent* paletteComponent = d->getCacheComponent(pi);
    if  ( !paletteComponent )
        return nullptr;

    CodePalette* cp = paletteComponent->getItem(d->engine->engine());
    if ( !cp )
        return nullptr;

    cp->setPath(QString::fromStdString(pi.path().data()));
    d->engine->engine()->setObjectOwnership(cp, QQmlEngine::JavaScriptOwnership);
    return cp;
}

QJSValue PaletteLoader::paletteContent(const PaletteContainer::PaletteInfo &pi){
    Q_D(PaletteLoader);
    PaletteComponent* paletteComponent = d->getCacheComponent(pi);
    if  ( !paletteComponent )
        return QJSValue();

    return paletteComponent->getContent(d->engine);
}

bool PaletteLoader::configuresLayout(const PaletteContainer::PaletteInfo &pi){
    return pi.extension() == "json";
}

}// namespace

