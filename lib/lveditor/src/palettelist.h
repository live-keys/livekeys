#ifndef LVPALETTELIST_H
#define LVPALETTELIST_H

#include <QObject>
#include <QAbstractListModel>
#include "lveditorglobal.h"
#include "live/codepalette.h"

namespace lv{

class PaletteLoader;
class PaletteContainer;
class LV_EDITOR_EXPORT PaletteList : public QAbstractListModel{

    Q_OBJECT

public:
    explicit PaletteList(PaletteContainer* container, QObject *parent = nullptr);
    ~PaletteList();

    void setPosition(int position);

    QVariant data(const QModelIndex& index, int role) const;
    int rowCount(const QModelIndex& parent) const;
    QHash<int, QByteArray> roleNames() const;

    void append(PaletteLoader* loader);
    void remove(PaletteLoader* loader);
    PaletteLoader* loaderAt(int index);

public slots:
    int position() const;
    int size() const;
    lv::CodePalette *loadAt(int index);

private:
    int                       m_position;
    PaletteContainer*     m_paletteContainer;
    QList<PaletteLoader*> m_palettes;
};

inline void PaletteList::setPosition(int position){
    m_position = position;
}

inline int PaletteList::rowCount(const QModelIndex &) const{
    return size();
}

inline void PaletteList::remove(PaletteLoader *loader){
    m_palettes.removeAll(loader);
}

inline PaletteLoader *PaletteList::loaderAt(int index){
    return m_palettes.at(index);
}

inline int PaletteList::position() const{
    return m_position;
}

inline int PaletteList::size() const{
    return m_palettes.size();
}

}// namespace

#endif // LVPALETTELIST_H
