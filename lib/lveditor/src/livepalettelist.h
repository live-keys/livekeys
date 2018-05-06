#ifndef LVLIVEPALETTELIST_H
#define LVLIVEPALETTELIST_H

#include <QObject>
#include <QAbstractListModel>
#include "lveditorglobal.h"
#include "live/livepalette.h"

namespace lv{

class LivePaletteLoader;
class LivePaletteContainer;
class LV_EDITOR_EXPORT LivePaletteList : public QAbstractListModel{

    Q_OBJECT

public:
    explicit LivePaletteList(LivePaletteContainer* container, QObject *parent = nullptr);
    ~LivePaletteList();

    QVariant data(const QModelIndex& index, int role) const;
    int rowCount(const QModelIndex& parent) const;
    QHash<int, QByteArray> roleNames() const;

    void append(LivePaletteLoader* loader);

public slots:
    int size() const;
    lv::LivePalette *loadAt(int index);

private:
    LivePaletteContainer*     m_paletteContainer;
    QList<LivePaletteLoader*> m_palettes;
};

inline int LivePaletteList::rowCount(const QModelIndex &) const{
    return size();
}

inline int LivePaletteList::size() const{
    return m_palettes.size();
}

}// namespace

#endif // LIVEPALETTELIST_H
