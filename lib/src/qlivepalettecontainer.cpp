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

class QLivePaletteContainerPrivate{
public:
    QQmlEngine* engine;
    QHash<QString, QHash<QString, QCodeConverter*> > m_items;
};

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
    QString palettePath = QDir::cleanPath(path + "/" + "palettedir");

    QPALETTE_CONTAINER_DEBUG("Scanning palettedir: " + palettePath);

    QFile f(palettePath);
    if ( !f.open(QIODevice::ReadOnly) ){
        qWarning("Failed to read palettedir file: %s", qPrintable(palettePath));
        return;
    }

    while ( !f.atEnd() ){
        QByteArray line = f.readLine().trimmed();
        if ( !line.isEmpty() ){
            loadPalette(QDir::cleanPath(path + "/" + line));
        }
    }
}

void QLivePaletteContainer::loadPalette(const QString &path){
    Q_D(QLivePaletteContainer);

    QPALETTE_CONTAINER_DEBUG("Loading palette: " + path);

    QQmlComponent component(d->engine, QUrl::fromLocalFile(path), QQmlComponent::PreferSynchronous);
    if ( component.isError() || component.isNull() ){
        qCritical("Failed to load palette: %s", qPrintable(path));
        handleError(component);
        return;
    }

    QObject *obj = component.create();
    if ( obj == 0 || component.isError() ){
        qCritical("Failed to create object from palette: %s", qPrintable(path));
        handleError(component);
        return;
    }

    QCodeConverter* cvt = static_cast<lcv::QCodeConverter*>(obj);
    if ( !cvt ){
        qCritical("Failed to load palette: \'%s\'. Value is not a LivePalette or CodeConverter type.",
                  qPrintable(path));
        return;
    }

    if ( cvt->type().isEmpty() || cvt->serialize() == 0 ){
        qCritical("Failed to load palette: \'%s\'. Palette/CodeConverter must have a type and serializer defined",
                  qPrintable(path));
        return;
    }

    d->m_items[cvt->type()][cvt->typeObject()] = cvt;
    QPALETTE_CONTAINER_DEBUG("Loaded palette on type: " + cvt->type() + " " + cvt->typeObject());
}

QLivePaletteContainer *QLivePaletteContainer::create(QQmlEngine *engine, const QString &path){
    QLivePaletteContainer* ct = new QLivePaletteContainer(engine);
    ct->scanPalettes(path);
    return ct;
}

int QLivePaletteContainer::size() const{
    Q_D(const QLivePaletteContainer);
    return d->m_items.size();
}

void QLivePaletteContainer::handleError(const QQmlComponent &component) const{
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



}// namespace

