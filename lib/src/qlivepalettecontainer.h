#ifndef QPALLETECONTAINER_H
#define QPALLETECONTAINER_H

#include "qliveglobal.h"

class QQmlEngine;
class QQmlComponent;

namespace lcv{

class QLivePaletteContainerPrivate;
class Q_LIVE_EXPORT QLivePaletteContainer{

public:
    ~QLivePaletteContainer();

    void scanPalettes(const QString& path);
    void scanPaletteDir(const QString& path);

    void loadPalette(const QString& path);

    static QLivePaletteContainer* create(QQmlEngine* engine, const QString& path);

    int size() const;

private:
    QLivePaletteContainer(QQmlEngine* engine);

    Q_DISABLE_COPY(QLivePaletteContainer)
    Q_DECLARE_PRIVATE(QLivePaletteContainer)

    QLivePaletteContainerPrivate* const d_ptr;
};

}// namespace

#endif // QPALLETECONTAINER_H
