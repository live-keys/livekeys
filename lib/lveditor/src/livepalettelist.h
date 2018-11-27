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

    void setPosition(int position);

    QVariant data(const QModelIndex& index, int role) const;
    int rowCount(const QModelIndex& parent) const;
    QHash<int, QByteArray> roleNames() const;

    void append(LivePaletteLoader* loader);
    void remove(LivePaletteLoader* loader);
    LivePaletteLoader* loaderAt(int index);

public slots:
    int position() const;
    int size() const;
    lv::LivePalette *loadAt(int index);

private:
    int                       m_position;
    LivePaletteContainer*     m_paletteContainer;
    QList<LivePaletteLoader*> m_palettes;
};

inline void LivePaletteList::setPosition(int position){
    m_position = position;
}

inline int LivePaletteList::rowCount(const QModelIndex &) const{
    return size();
}

inline void LivePaletteList::remove(LivePaletteLoader *loader){
    m_palettes.removeAll(loader);
}

inline LivePaletteLoader *LivePaletteList::loaderAt(int index){
    return m_palettes.at(index);
}

inline int LivePaletteList::position() const{
    return m_position;
}

inline int LivePaletteList::size() const{
    return m_palettes.size();
}

}// namespace

#endif // LIVEPALETTELIST_H
