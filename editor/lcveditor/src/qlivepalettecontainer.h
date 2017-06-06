#ifndef QLIVEPALLETECONTAINER_H
#define QLIVEPALLETECONTAINER_H

#include "qlcveditorglobal.h"
#include <QString>

class QQmlEngine;
class QQmlComponent;

namespace lcv{

class QCodeConverter;
class QLivePaletteContainerPrivate;
class Q_LCVEDITOR_EXPORT QLivePaletteContainer{

public:
    ~QLivePaletteContainer();

    void scanPalettes(const QString& path);
    void scanPaletteDir(const QString& path);

    static QLivePaletteContainer* create(QQmlEngine* engine, const QString& path);

    QCodeConverter* findPalette(const QString& type, const QString& object = "");
    QCodeConverter* findPalette(const QString &type, const QStringList &object);

    int size() const;

private:
    QLivePaletteContainer(QQmlEngine* engine);

    Q_DISABLE_COPY(QLivePaletteContainer)
    Q_DECLARE_PRIVATE(QLivePaletteContainer)

    QLivePaletteContainerPrivate* const d_ptr;
};

}// namespace

#endif // QLIVEPALLETECONTAINER_H
